#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <type_traits>

#include "gfx/rio_Window.h"
#include "filedevice/rio_FileDeviceMgr.h"
#include "gfx/rio_PrimitiveRenderer.h"
#include "gpu/rio_RenderBuffer.h"
#include "gpu/rio_RenderTarget.h"

#include "rio-e/Types/Scene.h"

#include "rio-e/Common/Time.h"
#include "rio-e/Common/Input.h"
#include "rio-e/Common/Network.h"

#include "rio-e/Systems/ModelLoader.h"

#if RIO_IS_CAFE
#include <gx2/clear.h>
#include <gx2/display.h>
#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/registers.h>
#include <gx2/state.h>
#include <gx2/swap.h>
#include <proc_ui/procui.h>
#endif // RIO_IS_CAFE


namespace rioe
{
	class Engine
	{
	public:
		enum DrawType
		{
			DRAW_TYPE_DRC = 0,
			DRAW_TYPE_TV,
			DRAW_TYPE_MAX
		};

	public:
		static Engine& Instance() {
			static Engine instance;
			return instance;
		}

		// Delete copy/move constructors and assignment operators
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		Engine() : mWindow(nullptr), mFileDeviceMgr(nullptr) {};

		void Start(const char* sceneName)
		{
			RIO_LOG("[RIO(e)] Application start\n");

			rio::FileDeviceMgr::createSingleton();
			RIO_LOG("[RIO(e)] FileDeviceMgr created\n");

			rio::Window::createSingleton(1280, 720);
			RIO_LOG("[RIO(e)] Window created\n");

			mFileDeviceMgr = rio::FileDeviceMgr::instance();
			mWindow = rio::Window::instance();

#ifdef WINDOW_NAME
			SetWindowName(WINDOW_NAME);
#endif

			InitializeRenderBuffers();

			mCurSceneName = sceneName;

			rioe::Network::Initialize();

			mSceneList[mCurSceneName]->Start();

			while (mWindow->isRunning())
			{
				LogicStep();

				DrawStep();
			}
		};

		template <typename T>
		void AddScene(const char* sceneName)
		{
			static_assert(std::is_base_of<IScene, T>::value, "T must inherit from base class IScene.");

			T* newScene = new T();
			mSceneList[sceneName] = newScene;
		}

		void RequestExitScene(const char* sceneName)
		{
			if (mSceneList.size() >= 1 && mSceneList.at(mCurSceneName))
			{
				RIO_LOG("[RIO(e)] Requesting %s to exit..\n", sceneName);
				mSceneList[sceneName]->Exit();
				mSceneList[sceneName]->ClearNodes();
				rioe::ModelLoader::Cleanup();

				RIO_LOG("[RIO(e)] %s exited.\n", sceneName);

				mCurSceneName.clear();
			}
		}

		void ChangeActiveScene(const char* sceneName)
		{
			if (!mSceneList.size() >= 1)
				return;

			if (!mCurSceneName.empty())
				RequestExitScene(mCurSceneName.c_str());

			mCurSceneName = sceneName;
			mSceneList[mCurSceneName]->Start();
		}

		void Exit()
		{
			if (!mCurSceneName.empty())
				RequestExitScene(mCurSceneName.c_str());

			for (auto& entry : mSceneList)
			{
				delete entry.second;
				entry.second = nullptr;
			}

			mSceneList.clear();

			ModelLoader::Cleanup();

			for (int i = 0; i < DRAW_TYPE_MAX; i++)
			{
				delete mColorRenderTextures[i];
				delete mDepthRenderTextures[i];

				delete mRenderBuffers[i];
			}

			rio::FileDeviceMgr::destroySingleton();
			rio::Window::destroySingleton();
		};

		void SetWindowName(const char* windowName)
		{
#if RIO_IS_WIN
			glfwSetWindowTitle(mWindow->getNativeWindow().getGLFWwindow(), windowName);
#else
			RIO_LOG("[RIO(e)] Window name changing stubbed. %s\n", windowName);
#endif // RIO_IS_WIN
		}
	public:
		rioe::IScene* GetActiveScene() { return mSceneList[mCurSceneName]; }

		const std::string& GetActiveSceneName() { return mCurSceneName; }

		const std::unordered_map<std::string, IScene*>& GetSceneList() { return mSceneList; };

		DrawType GetCurrentDrawType() { return mCurrentDrawType; };
	private:
		rio::Window* mWindow;
		rio::FileDeviceMgr* mFileDeviceMgr;

		std::string mCurSceneName;
		std::unordered_map<std::string, IScene*> mSceneList;

		DrawType mCurrentDrawType = DRAW_TYPE_TV;

		rio::RenderBuffer* mRenderBuffers[DRAW_TYPE_MAX];
		rio::RenderTargetColor mRenderTargetsColor[DRAW_TYPE_MAX];
		rio::RenderTargetDepth mRenderTargetsDepth[DRAW_TYPE_MAX];

		rio::Texture2D* mColorRenderTextures[DRAW_TYPE_MAX];
		rio::Texture2D* mDepthRenderTextures[DRAW_TYPE_MAX];

		void LogicStep()
		{
			rioe::Input::Calc();
			rioe::Time::Calc();

			mSceneList[mCurSceneName]->UpdateStep();

			for (const auto& node : *mSceneList[mCurSceneName]->GetAllNodes())
			{
				for (const auto& property : node.second->mProperties)
				{
					property->UpdateStep();
				}
			}

			mSceneList[mCurSceneName]->LateUpdateStep();
		};

		void DrawStep()
		{
#if RIO_IS_CAFE
			mRenderBuffers[DRAW_TYPE_TV]->setSize(1280, 720);
			mRenderBuffers[DRAW_TYPE_DRC]->setSize(854, 480);

			for (int i = 0; i < DRAW_TYPE_MAX; i++)
			{
				mCurrentDrawType = static_cast<DrawType>(i);
				
				mRenderBuffers[i]->bind();

				mSceneList[mCurSceneName]->DrawStep();

				for (const auto& node : *mSceneList[mCurSceneName]->GetAllNodes())
				{
					for (const auto& property : node.second->mProperties)
					{
						property->DrawStep();
					}
				}

				mSceneList[mCurSceneName]->LateDrawStep();
			}

			// Reimplemenation of window->swapBuffers()

			rio::RenderState render_state;
			render_state.setDepthTestEnable(false);
			render_state.apply();

			GX2Flush();

			GX2CopyColorBufferToScanBuffer(&mRenderTargetsColor[DRAW_TYPE_TV].getNativeColorBuffer(), GX2_SCAN_TARGET_TV);
			GX2CopyColorBufferToScanBuffer(&mRenderTargetsColor[DRAW_TYPE_DRC].getNativeColorBuffer(), GX2_SCAN_TARGET_DRC);

			GX2SwapScanBuffers();

			GX2SetContextState(mWindow->getNativeWindow().getContextState());

			GX2Flush();
			GX2SetTVEnable(true);
			GX2SetDRCEnable(true);
			GX2WaitForFlip();

			ProcUIStatus status = ProcUIProcessMessages(true);
			ProcUIStatus previous_status = status;

			if (status == PROCUI_STATUS_RELEASE_FOREGROUND)
			{
				const_cast<rio::Window*>(mWindow)->foregroundRelease_();
				status = ProcUIProcessMessages(true);
			}

			if (status == PROCUI_STATUS_EXITING ||
				(previous_status == PROCUI_STATUS_RELEASE_FOREGROUND &&
					status == PROCUI_STATUS_IN_FOREGROUND &&
					!const_cast<rio::Window*>(mWindow)->foregroundAcquire_()))
			{
				ProcUIShutdown();
				const_cast<rio::Window*>(mWindow)->terminate_();
			}
#else
			mSceneList[mCurSceneName]->DrawStep();

			for (const auto& node : *mSceneList[mCurSceneName]->GetAllNodes())
			{
				for (const auto& property : node.second->mProperties)
				{
					property->DrawStep();
				}
			}

			mSceneList[mCurSceneName]->LateDrawStep();

			mWindow->swapBuffers();
#endif // RIO_IS_CAFE

			
		};

		void InitializeRenderBuffers()
		{
			mRenderBuffers[DRAW_TYPE_TV] = new rio::RenderBuffer;
			mRenderBuffers[DRAW_TYPE_DRC] = new rio::RenderBuffer;

			mColorRenderTextures[DRAW_TYPE_TV] = new rio::Texture2D(rio::TEXTURE_FORMAT_R8_G8_B8_A8_UNORM, mWindow->getWidth(), mWindow->getHeight(), 0);
			mDepthRenderTextures[DRAW_TYPE_TV] = new rio::Texture2D(rio::DEPTH_TEXTURE_FORMAT_R32_FLOAT, mWindow->getWidth(), mWindow->getHeight(), 0);
			mRenderTargetsColor[DRAW_TYPE_TV].linkTexture2D(*mColorRenderTextures[DRAW_TYPE_TV]);
			mRenderTargetsDepth[DRAW_TYPE_TV].linkTexture2D(*mDepthRenderTextures[DRAW_TYPE_TV]);

			mColorRenderTextures[DRAW_TYPE_DRC] = new rio::Texture2D(rio::TEXTURE_FORMAT_R8_G8_B8_A8_UNORM, 854, 480, 0);
			mDepthRenderTextures[DRAW_TYPE_DRC] = new rio::Texture2D(rio::DEPTH_TEXTURE_FORMAT_R32_FLOAT, 854, 480, 0);
			mRenderTargetsColor[DRAW_TYPE_DRC].linkTexture2D(*mColorRenderTextures[DRAW_TYPE_DRC]);
			mRenderTargetsDepth[DRAW_TYPE_DRC].linkTexture2D(*mDepthRenderTextures[DRAW_TYPE_DRC]);

			mRenderBuffers[DRAW_TYPE_TV]->setRenderTargetColor(&mRenderTargetsColor[DRAW_TYPE_TV]);
			mRenderBuffers[DRAW_TYPE_TV]->setRenderTargetDepth(&mRenderTargetsDepth[DRAW_TYPE_TV]);
			mRenderBuffers[DRAW_TYPE_DRC]->setRenderTargetColor(&mRenderTargetsColor[DRAW_TYPE_DRC]);
			mRenderBuffers[DRAW_TYPE_DRC]->setRenderTargetDepth(&mRenderTargetsDepth[DRAW_TYPE_DRC]);

			mRenderBuffers[DRAW_TYPE_TV]->setSize(mWindow->getWidth(), mWindow->getHeight());
			mRenderBuffers[DRAW_TYPE_DRC]->setSize(854, 480);
		};
	};

	inline Engine* GetEngine()
	{
		return &Engine::Instance();
	}
}