class Foo {
public:
    Foo() {
    }

    void first(function<void()> printFirst) {   
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();

        processedFirst.store(true, std::memory_order_release);
    }

    void second(function<void()> printSecond) {
		while (!processedFirst.load()) {
			std::this_thread::yield();
		}

        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();

        processedSecond.store(true, std::memory_order_release);
    }

    void third(function<void()> printThird) {
		while (!processedSecond.load()) {
			std::this_thread::yield();
		}

        // printThird() outputs "third". Do not change or remove this line.
        printThird(); 
    }

    std::atomic<bool> processedFirst{ false };
    std::atomic<bool> processedSecond{ false };
};