#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <type_traits>

#include "gfx/rio_Window.h"
#include "filedevice/rio_FileDeviceMgr.h"

#include "rio-e/Types/Scene.h"

#include "rio-e/Common/Time.h"
#include "rio-e/Common/Input.h"
#include "rio-e/Common/Network.h"

#include "rio-e/Systems/ModelLoader.h"

#include "gfx/rio_PrimitiveRenderer.h"

namespace rioe
{
	class Engine
	{
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
	private:
		rio::Window* mWindow;
		rio::FileDeviceMgr* mFileDeviceMgr;

		std::string mCurSceneName;
		std::unordered_map<std::string, IScene*> mSceneList;

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
		};
	};

	inline Engine* GetEngine()
	{
		return &Engine::Instance();
	}
}