#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

class NonCopyable
{
public:
    NonCopyable() = default;
protected:
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

#endif // __NONCOPYABLE_H__
