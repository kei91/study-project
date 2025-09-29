class Foo {
public:
    Foo() {
    }

    void first(function<void()> printFirst) {   
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();

        processedFirst = true;
        cv.notify_all();
    }

    void second(function<void()> printSecond) {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [this] { return processedFirst; });

        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();

        processedSecond = true;
        lk.unlock();
        cv.notify_all();
    }

    void third(function<void()> printThird) {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [this] { return processedSecond; });

        // printThird() outputs "third". Do not change or remove this line.
        printThird(); 
        lk.unlock();
    }

    std::mutex m;
    std::condition_variable cv;

    bool processedFirst = false;
    bool processedSecond = false;
};