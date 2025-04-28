//
// Created by Administrator on 2025/4/28.
//

#pragma once
#include <QObject>
#include <any>
// 基础处理器接口
class HandlerBase {
public:
    virtual ~HandlerBase() = default;
    virtual void invoke(const std::vector<std::any>& args) = 0;
};

// 特定类型处理器实现
template<typename... Args>
class Handler : public HandlerBase {
private:
    std::function<void(Args...)> m_func;

    // 辅助函数：将std::any转换为指定类型
    template<typename T>
    T convertArg(const std::any& arg, int index) {
        try {
            return std::any_cast<T>(arg);
        } catch (const std::bad_any_cast&) {
            qWarning() << "Cannot convert argument" << index << "to required type";
            return T();
        }
    }

    // 辅助函数：展开参数包并调用函数
    template<std::size_t... I>
    void callWithArgs(const std::vector<std::any>& args, std::index_sequence<I...>) {
        if (args.size() < sizeof...(Args)) {
            qWarning() << "Not enough arguments provided";
            return;
        }
        m_func(convertArg<std::decay_t<Args>>(args[I], I)...);
    }


public:
    explicit Handler(std::function<void(Args...)> func) : m_func(std::move(func)) {}

    void invoke(const std::vector<std::any>& args) override {
        callWithArgs(args, std::index_sequence_for<Args...>{});
    }
};

class Sol3EventBus : public QObject {
    Q_OBJECT
public:
    explicit Sol3EventBus(QObject* parent = nullptr){};


    ~Sol3EventBus() =default;

    void detach(const QString& strID) {
        qInfo() << "Detaching handler for:" << strID;
        m_pHandlers.erase(strID);
    }



    // 注册事件处理函数 - 有参数版本
    template<typename... Args>
    void attach(const QString& strID, std::function<void(Args...)> pHandler) {
        qInfo() << "Attaching handler for:" << strID;
        m_pHandlers.emplace(strID, std::make_shared<Handler<Args...>>(std::move(pHandler)));
    }

    // 注册事件处理函数 - 无参数版本的特化
    void attach(const QString& strID, std::function<void()> pHandler) {
        qInfo() << "Attaching handler (no args) for:" << strID;
        m_pHandlers.emplace(strID, std::make_shared<Handler<>>(std::move(pHandler)));
    }

    // 支持直接传入 lambda 函数
    template<typename F>
    void attach(const QString& strID, F&& lambda) {
        attach(strID, std::function(std::forward<F>(lambda)));
    }


    // 发布事件
    template<typename... Args>
    void postEvent(const QString& strID, Args&&... args) {
     //   qInfo() << "Posting event:" << strID;

        // 将参数打包到std::vector<std::any>
        std::vector<std::any> anyArgs;
        (anyArgs.push_back(std::forward<Args>(args)), ...);

        // 查找并调用所有匹配的处理器
        auto range = m_pHandlers.equal_range(strID);
        for (auto it = range.first; it != range.second; ++it) {
            it->second->invoke(anyArgs);
        }
    };

private:
    std::multimap<QString, std::shared_ptr<HandlerBase>> m_pHandlers;
};


