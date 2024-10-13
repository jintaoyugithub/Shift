namespace shift {
	class AppBase {
	public:
		AppBase(const char* _name, const char* _description, const char* url = "") {};

		virtual ~AppBase() = 0;

		virtual void init(int _argc, const char** _argv, uint32_t width, uint32_t height) = 0;

		virtual bool update() = 0;
	
		virtual void shutdown() = 0;

		virtual void run(int _argc, const char** _argv) = 0;

	private:
		virtual bool bgfxInit(int _argc, const char ** _argv) = 0;

		virtual bool windowInit(uint32_t width, uint32_t height) = 0;

		virtual bool uiInit() = 0;
	};
} // namespace shift
