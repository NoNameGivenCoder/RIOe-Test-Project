namespace rioe
{
	class SaveMgr
	{
	public:
		static bool createSingleton();
		static bool destroySingleton();
	private:
		static SaveMgr* mInstance;
	};
}