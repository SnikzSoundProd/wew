// --- CREATE NEW FILE: src/core/Log.h ---
#pragma once
#include <vector>
#include <string>
#include <mutex> // Для потокобезопасности в будущем

// Уровни сообщений
enum class LogLevel {
    Info,
    Warning,
    Error
};

// Структура одного сообщения
struct LogMessage {
    std::string text;
    LogLevel level;
};

// Синглтон-класс для логирования
class Log {
public:
    // Запрещаем копирование
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    // Доступ к единственному экземпляру
    static Log& GetInstance() {
        static Log instance;
        return instance;
    }

    // Методы для добавления сообщений
    void Info(const std::string& message) {
        addMessage(message, LogLevel::Info);
    }

    void Warn(const std::string& message) {
        addMessage(message, LogLevel::Warning);
    }

    void Error(const std::string& message) {
        addMessage(message, LogLevel::Error);
    }

    // Очистка логов
    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messages.clear();
    }

    // Получение всех сообщений (для отрисовки)
    const std::vector<LogMessage>& GetMessages() const {
        return m_messages;
    }

private:
    Log() = default; // Приватный конструктор
    std::mutex m_mutex;
    std::vector<LogMessage> m_messages;

    void addMessage(const std::string& text, LogLevel level) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messages.push_back({ text, level });
    }
};