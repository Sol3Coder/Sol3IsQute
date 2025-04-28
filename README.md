# Sol3IsQute

自用qt控件、工具类，由于控件目前是特化版本，需要调整为通用的配色方案，后续添加，先上传一个简单的eventbus

sol3eventbus 用于解决项目中过多signal slot的问题，项目大了以后信号太多，核心的信号管理类写的信号越写越多，麻烦而且编译慢，实现eventbus，使用attach注册一个id及相应的逻辑处理lambda，当数据更新时，postevent即可。免去了绑定signal slot的麻烦
