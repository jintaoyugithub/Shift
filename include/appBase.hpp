namespace shift {

class AppBase {
public:
  virtual ~AppBase() = 0;
  virtual void init() = 0;
  virtual void update() = 0;
  virtual void shutdown() = 0;
};

int runApp(AppBase *_app, int _argc, char **_agrv);
} // namespace shift
