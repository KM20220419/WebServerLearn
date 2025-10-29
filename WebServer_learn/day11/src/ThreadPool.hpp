#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>
#include <functional>

class ThreadPool
{
public:
    ThreadPool(int size);
    ~ThreadPool();

    // 每个线程要做的事
    void thread_work();

    // 加入任务队列中的任务
    template <typename F, typename... Arg>
    auto task(F &&f, Arg &&...arg) -> std::future<typename std::result_of<F(Arg...)>::type>;

private:
    // 线程池的最大数量
    int maxsize;

    // 线程池容器
    std::vector<std::thread> threads;

    // 任务队列
    std::queue<std::function<void()>> taskqueue;

    // 是否停止线程池
    bool isStop;

    // 定义互斥锁
    std::mutex mtx;

    // 定义条件变量
    std::condition_variable cv;
};

ThreadPool::ThreadPool(int size) : isStop(false)
{
    if (size < 1)
        throw std::runtime_error("size error");
    for (int i = 0; i < size; i++)
    {
        // 因为push_back不能构造对象，所以要传入临时对象.
        // 而emplace_back可以自己构造所需类型的临时对象，所以能直接传入参数
        // push_back 是“把已有对象放进去”，不能直接传构造参数；
        // emplace_back 是“在容器里直接构造对象”，可以直接传构造参数。
        threads.emplace_back([this]()
                             { this->thread_work(); });
    }
}

ThreadPool::~ThreadPool()
{
    // 停止线程池
    {
        std::unique_lock<std::mutex> umtx(mtx);
        isStop = true;
    }

    // 通知所有阻塞的现场，完成任务队列中的人物
    cv.notify_all();

    // 设置join
    for (auto &thread : threads)
    {
        if (thread.joinable())
            thread.join();
    }
}

// 定义函数以及可变参数的模板
template <class F, class... Arg>
// std::future<typename std::result_of<F(Arg...)>::type>表示自动推导出函数的返回值类型
auto ThreadPool::task(F &&f, Arg &&...arg) -> std::future<typename std::result_of<F(Arg...)>::type>
{
    // 使用一个变量名来代替返回值类型
    using funtypename = typename std::result_of<F(Arg...)>::type;

    // std::bind(std::forward<F>(f), std::forward<Arg>(args)...);将有参函数绑定为无参函数
    // 使用packaged_task可以将可调用对象包装起来，funtypename()表示返回值类型为funtypename，并且()填写参数类型，无参则为空
    // 这样不仅可以在之后调用执行，
    // 而且可以通过get_future以及get两个联合得到函数返回值（如果有）
    // 由于std::packaged_back不可拷贝，只能移动
    auto task_ptr = std::make_shared<std::packaged_task<funtypename()>>(std::bind(std::forward<F>(f), std::forward<Arg>(arg)...));
    auto resfuture = task_ptr->get_future();
    {
        std::unique_lock<std::mutex> uqmtx(mtx);
        // 放入任务队列中，上面使用共享指针，是为了保证lambda可以捕捉到这个指针，并且保证该指针可以共享
        // 以免该函数结束时，被释放
        if (isStop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        taskqueue.emplace([task_ptr]()
                          { (*task_ptr)(); });
    }

    cv.notify_one();
    // 返回resfuture 通过resfuture.get() 得到函数返回值
    return resfuture;
}

void ThreadPool::thread_work()
{
    while (true)
    {
        std::function<void()> restask;
        {
            std::unique_lock<std::mutex> umtx(mtx);

            // 阻塞的条件：线程池开启(未停止)并且任务队列为空 ---> 这时整个条件判断为false ,阻塞
            cv.wait(umtx, [this]()
                    { return this->isStop || !this->taskqueue.empty(); });
            // 如果线程池关闭了，并且任务队列为空，则退出
            if (isStop && taskqueue.empty())
                return;

            // 取出任务，执行
            // 使用移动语义
            restask = std::move(this->taskqueue.front());
            this->taskqueue.pop();
        }
        restask();
    }
}
