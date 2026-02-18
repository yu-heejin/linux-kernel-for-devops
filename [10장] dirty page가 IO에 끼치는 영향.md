# 10.1 dirty page란

리눅스에서 파일 I/O가 일어날 때 커널은 PageCache를 이용해서 디스크에 있는 파일의 내용을 메모리에 잠시 저장하고, 필요할 때마다 메모리에 접근해서 사용한다. 이를 통해 디스크보다 더 빠른 메모리의 접근 속도를 활용할 수 있고 전체적으로 시스템의 성능을 향상시킬 수 있다.

그림을 보면 각각 a, b, c 파일이 PageCache에 올라가 있으며 1:1로 대응하고 있다. 이 상태에서 b 파일에 쓰기 작업이 이루어졌다고 가정해보자. b 파일의 내용이 b가 아니라 d로 바뀌었으며, 이 상태에서 보면 b 파일의 내용은 디스크와 PageCache에 있는 내용이 서로 다르다. 그래서 **커널은 해당 메모리 영역에 대해 디스크에 있는 내용과 달라졌음을 표시하는 Dirty 비트를 켜고 바로 이 영역은 dirty page라고 부른다.** 즉, dirty page는 **PageCache에 있는 페이지 중에서도 쓰기 작업이 이루어진 메모리**라고 볼 수 있다. 이 상태에서는 아직 PageCache와 디스크 사이의 내용이 다르기 때문에 전원 장애 등으로 시스템이 꺼져 버린다면 미처 디스크에 쓰지 못한 내용은 날아가게 되며 파일의 내용에 대한 정합성이 깨지게 된다. 하지만 dirty page로 표시된 메모리들을 dirty page가 생성될 때마다 디스크에 쓰면 이 또한 상당량의 쓰기 I/O를 일으켜서 성능을 저하시킬 수 있다.

**그래서 커널은 몇 가지 조건을 만들어서 해당 조건을 만족시키면 dirty page를 디스크로 동기화한다. 이 과정을 page writeback이라고 부르며 dirty page 동기화**라고도 한다(리눅스 커널은 디스크의 데이터를 캐싱하는데, 이를 페이지 캐시(page cache)라고 하고, 캐시되어 있던 페이지가 다시 디스크로 적용되는 것(동기화 되는 것)을 page writeback이라고 한다.). 커널 버전에 따라서 다르겠지만 보통 flush라는 단어가 들어간 커널 스레드(pdflush, flush, bdflush 등)가 이 작업을 진행한다. 그래서 **I/O가 많이 발생하는 서버에서는 dirty page를 언제 얼마나 동기화 시키느냐가 중요한 성능 튜닝의 요소**가 된다. 그리고 커널에서는 서버의 워크로드에 따라 가장 적합한 동기화 전략을 구사할 수 있도록 파라미터로 조절할 수 있는 인터페이스를 제공하고 있다.

이 질문 진짜 잘했다 👍

여기서 헷갈리면 page cache 개념이 완전히 꼬여.

---

# 🔥 핵심 먼저

> ❗ dirty page는
> 
> 
> **디스크보다 메모리(page cache)가 최신 상태일 때 생긴다**
> 

그래서

> ✅ 동기화는 “메모리 → 디스크” 방향이다
> 
> 
> ❌ 디스크 → 캐시가 아니다
> 

---

# 🧠 구조를 정확히 보자

리눅스 파일 I/O 흐름은 이렇게 된다:

```
디스크  <---->  페이지 캐시(메모리)  <---->  프로세스
```

---

## 1️⃣ 파일을 읽을 때

1. 디스크 → 메모리(page cache)로 올라옴
2. 프로세스는 메모리에서 읽음

👉 이때는 clean page

---

## 2️⃣ 파일을 쓸 때

예:

```c
write(fd, "hello", 5);
```

이게 발생하면:

1. 디스크에 바로 쓰지 않음
2. 페이지 캐시에 먼저 씀
3. 메모리 내용이 디스크와 달라짐

👉 이 상태가 바로 **dirty page**

---

# 📌 그러면 왜 디스크로 동기화하냐?

왜냐면

> 디스크가 영구 저장소이기 때문
> 

메모리는:

- 전원 끄면 날아감
- 커널 패닉 나면 날아감

그래서

> 결국은 디스크에 반영해야 안전함
> 

---

# 💥 네가 헷갈린 부분

> “캐시랑 디스크가 다르면 캐시를 디스크에 맞춰야 하는 거 아니야?”
> 

아니야.

리눅스는 **write-back 캐시 전략**을 쓴다.

즉:

- 메모리가 진짜 데이터
- 디스크는 나중에 따라감

---

# 🎯 왜 이렇게 설계했을까?

### 이유 1️⃣ 성능

디스크는 느림

메모리는 빠름

매 write마다 디스크 쓰면 서버 터짐

그래서:

- 메모리에 쌓아두고
- 한 번에 묶어서 디스크로 씀

→ 이게 page writeback

---

# 🧩 그래서 dirty page 동기화 전략이 중요

커널은 이런 기준으로 flush함:

- dirty 비율이 일정 % 넘으면
- 일정 시간 지나면
- 메모리 부족하면
- sync/fsync 호출되면

---

# 🔧 관련 튜닝 파라미터

확인해봐:

```bash
sysctl -a | grep dirty
```

중요한 것들:

```
vm.dirty_ratio
vm.dirty_background_ratio
vm.dirty_expire_centisecs
vm.dirty_writeback_centisecs
```

---

# 🔥 요약 한 문장

> dirty page는 메모리가 더 최신이기 때문에
> 
> 
> 결국 디스크를 메모리 상태로 맞춰주는 과정이 writeback이다.
> 

---

# 🎓 한 줄 비유

디스크 = 공식 기록부

메모리 = 수정 중인 초안

dirty page = 아직 기록부에 반영 안 된 수정본

writeback = 초안을 공식 기록부에 옮기는 과정

---

원하면 내가:

- dirty_ratio가 낮으면 무슨 일이 벌어지는지
- write-heavy 서버에서 dirty 튜닝 전략
- DB 서버에서 왜 O_DIRECT를 쓰는지

까지 이어서 정리해줄까?

# 10.2 dirty page 관련 커널 파라미터

```yaml
root@ip-172-31-0-19:/home/ubuntu# sysctl -a | grep -i dirty
vm.dirty_background_bytes = 0
vm.dirty_background_ratio = 10
vm.dirty_bytes = 0
vm.dirty_expire_centisecs = 3000
vm.dirty_ratio = 20
vm.dirty_writeback_centisecs = 500
vm.dirtytime_expire_seconds = 43200
```

## vm.dirty_background_ratio

**dirty page의 내용을 백그라운드로 동기화할 때 그 기준이 되는 비율을 의미한다.** 전체 메모리 양에 해당 파라미터에 설정되어 있는 비율을 곱해서 나온 기준 값보다 dirty page의 크기가 커지면 백그라운드에서 dirty page의 내용을 디스크로 동기화한다. 만약 이 값이 10이고 전체 메모리가 16GB라고 가정한다면, dirty page의 크기가 1.6GB가 되었을 때 백그라운드 동기화를 진행한다.

## vm.dirty_background_bytes

ratio가 전체 메모리 대비 비율을 의미하지만, bytes는 절대적인 bytes의 값을 의미한다. 만약 이 값이 65535라면 dirty page의 크기가 65535bytes가 되었을 때 동기화한다.

## vm.dirty_ratio

전체 메모리를 기준으로 dirty page의 비율을 산정하지만 background라는 단어가 빠져 있음을 눈여겨봐야 한다. 만약 이 값이 10으로 설정되어 있고 전체 메모리가 16GB라고 가정하면, A라는 프로세스가 I/O 작업을 하던 중 dirty 페이지의 크기가 1.6GB가 되면 해당 프로세스의 I/O 작업을 모두 멈추게 하고 dirty page를 동기화한다.

## vm.dirty_bytes

`vm.dirty_ratio` 와 같은 방식으로 사용되지만 비율이 아니라 절대적인 bytes 양을 기준으로 한다.

## vm.dirty_writeback_centisecs

flush 커널 스레드를 몇 초 간격으로 깨울 것인지를 결정한다. 설정되는 값은 1/100초 값이기 때문에 5초로 설정하고 싶다면 500 * (1/100)으로 표시한다. 이렇게 하면 5초에 한 번 flush 커널 스레드가 꺠어나서 동기화하게 된다.

## vm.dirty_expire_centisecs

이 값도 flush 커널 스레드의 동작에 영향을 미친다. `vm.dirty_writeback_centisecs` 값에 의해 꺠어난 flush 커널 스레드가 디스크에 싱크시킬 dirty page의 기준을 찾을 때 이 값을 사용한다. 이 값도 1/100초 값이며 만약 3000으로 표시되어 있다면 30초를 의미한다. 그래서 dirty page로 분류된 후 30초 동안 디스크로 동기화되지 않은 페이지들을 디스크에 동기화시킨다.

# 10.3 백그라운드 동기화

## 백그라운드 동기화

동기화 작업이 백그라운드로 진행되는 것을 의미한다. 커널 파라미터 중에 `vm.dirty_background_ratio`와 `vm.dirty_ratio`를 통해서 조절할 수 있다. 엄밀히 말하면 `vm.dirty_ratio` 값을 넘어섰을 때 발생하는 동기화는 백그라운드 동기화가 아니지만, 명령어에 의해 명시적으로 이루어지는 동기화는 아니기 때문에 함께 분류했다. 이 작업은 애플리케이션이 dirty page를 생성할 때마다 현재까지 생성된 dirty page와 전체 메모리의 비율을 바탕으로 진행된다.

## 주기적인 동기화

동기화 작업이 주기적으로 진행되는 것을 말한다. 커널 파라미터 중에 `vm.dirty_writeback_centisec`, `vm.dirty_expire_centisecs`를 통해서 조절할 수 있다. 이 값들을 조절해서 dirty page를 동기화하기 위해 필요한 flush 데몬을 깨우는 주기와 깨웠을 때 동기화시키는 dirty page의 기준을 설정할 수 있다.

## 명시적인 동기화

명시적으로 동기화시키는 것을 의미한다. sync, fsync 등의 명령을 이용하면 현재 생성되어 있는 dirty page를 명시적으로 디스크에 쓰는데, 이런 작업을 명시적인 동기화라고 표현한다.

이번 절에서는 백그라운드 동기화와 주기적인 동기화를 살펴볼 것이다. 커널의 동작을 추적하기 위해 우선 ftrace를 이용할 수 있는 환경을 설정한다.

```yaml
ubuntu@ip-172-31-0-19:~$ sudo su
root@ip-172-31-0-19:/home/ubuntu# mount -t debugfs debugfs /sys/kernel/debug
mount: /sys/kernel/debug: debugfs already mounted on /sys/kernel/debug.
root@ip-172-31-0-19:/home/ubuntu# cd /sys/kernel/debug/tracing/
root@ip-172-31-0-19:/sys/kernel/debug/tracing# echo function > ./current_tracer
root@ip-172-31-0-19:/sys/kernel/debug/tracing# cat -v ./trace_pipe
   kworker/u30:0-11      [000] .....  2051.367456: wq_worker_running <-schedule
   kworker/u30:0-11      [000] .....  2051.367456: kthread_data <-wq_worker_running
   kworker/u30:0-11      [000] .....  2051.367456: _raw_spin_lock_irq <-worker_thread
   kworker/u30:0-11      [000] d....  2051.367456: process_one_work <-worker_thread
   kworker/u30:0-11      [000] d....  2051.367456: kick_pool <-process_one_work
   kworker/u30:0-11      [000] .....  2051.367456: flush_to_ldisc <-process_one_work
   kworker/u30:0-11      [000] .....  2051.367456: mutex_lock <-flush_to_ldisc
   kworker/u30:0-11      [000] .....  2051.367456: __cond_resched <-mutex_lock
```

먼저 백그라운드 동기화를 살펴보기 위해 주기적인 동기화가 일어나지 않도록 `vm.dirty_writeback_centisecs` 값을 0으로 설정한다. 그리고 백그라운드 동기화가 조금 더 빨리 일어날 수 있도록 `vm.dirty_background_ratio` 값은 1로 설정한다.

```yaml
ubuntu@ip-172-31-0-19:~$ sudo su
root@ip-172-31-0-19:/home/ubuntu# sysctl -w vm.dirty_writeback_centisecs=0
vm.dirty_writeback_centisecs = 0
root@ip-172-31-0-19:/home/ubuntu# sysctl -w vm.dirty_background_ratio=1
vm.dirty_background_ratio = 1
```

```yaml
root@ip-172-31-0-19:/home/ubuntu# ./dirty_page 
Write File - Current Size : 1024 KB
Write File - Current Size : 2048 KB
Write File - Current Size : 3072 KB
Write File - Current Size : 4096 KB
Write File - Current Size : 5120 KB
Write File - Current Size : 6144 KB
Write File - Current Size : 7168 KB
Write File - Current Size : 8192 KB
Write File - Current Size : 9216 KB
Write File - Current Size : 10240 KB
Write File - Current Size : 11264 KB
root@ip-172-31-0-19:/home/ubuntu# ./show_dirty.sh 
Dirty:              6352 kB
Dirty:              5956 kB
Dirty:              6356 kB
Dirty:              6496 kB
Dirty:              5956 kB
Dirty:              6356 kB
Dirty:              6308 kB
Dirty:              6384 kB
Dirty:              6492 kB
Dirty:              6096 kB
```

```yaml
Dirty:              6384 kB
Dirty:              6384 kB
Dirty:              6384 kB
Dirty:              6616 kB
Dirty:              6384 kB
Dirty:              6736 kB
Dirty:              6384 kB
Dirty:              6384 kB
Dirty:              6384 kB
Dirty:              6476 kB
Dirty:              6392 kB
Dirty:              6384 kB
Dirty:              6652 kB
Dirty:              6400 kB
Dirty:              6384 kB
Dirty:              6384 kB
Dirty:              6484 kB
Dirty:              5984 kB
Dirty:              6384 kB
Dirty:              6384 kB
Dirty:              6384 kB
Dirty:              6384 kB
Dirty:              6404 kB
Dirty:              6384 kB
Dirty:              6384 kB
Dirty:              6592 kB
Dirty:              5980 kB
Dirty:              6384 kB
Dirty:              6384 kB
Dirty:              6084 kB
Dirty:              6384 kB
Dirty:              6384 kB

# 너무 커서 볼 수가 없다 ㅠㅠ
root@ip-172-31-0-19:/sys/kernel/debug/tracing# cat -v ./trace_pipe | grep balance_dirty_pages
cat: ./trace_pipe: Device or resource busy
```

테스트 결과를 보면 64MB를 조금 넘긴 후에 백그라운드 동기화가 일어나서 dirty page가 없어진 것을 확인할 수 있다.

여기서 재밌는 함수를 확인할 수 있다. `balance_dirty_pages_ratelimited_nr()` 함수와 `balance_dirty_pages()`라는 함수가 보인다. ftrace의 결과를 `grep balance_dirty_pages` 명령으로 필터링해보면 다수의 `balance_dirty_pages_ratelimited_nr()` 함수가 호출된 후 중간 중간에 `balance_dirty_pages()` 함수가 호출되는 패턴이다. 그리고 `balance_dirty_pages_rattelimited_nr()` 함수는 `generic_file_buffered_write()` 함수에 의해 호출된다. 그래서 dirty page를 생성하는 모든 프로세스들은 쓰기 작업이 이루어질 때마다 `balance_dirty_pages_ratelimited_nr()` 함수를 호출하게 되며, 이 함수는 내부적으로 `balance_dirty_pages()` 함수를 호출한다는 것을 알 수 있다.

커널의 소스 코드를 확인해보자. 사실 dirty page가 생성될 때마다 시스템의 모든 dirty page를 검사하고 확인하는 과정을 거치면 오버헤드가 꽤 크기 때문에 일정 수준 이상이 되었을 때만 확인 과정을 거치도록 비율을 이용해 제한을 주는 함수이다. 그래서 `ratelimit`라는 변수의 값을 이용해 해당 프로세스가 생성하는 dirty page의 크기가 일정 수준을 넘어서면 그때서야 비로소 `balance_dirty_pages()` 함수를 호출해서 시스템의 모든 dirty page의 크기를 바탕으로 동기화가 필요한지 여부를 확인한다. 아주 적은 양의 dirty page를 생성했는데 전체 시스템의 dirty page 크기를 계산해서 비교하게 되면 그것은 그것대로 시스템에 부하를 일으킬 수 있기 때문이다. 그럼 초기 비교 대상이 되는 `ratelimit_pages` 값은 어떻게 결정될까? 같은 파일 안에 `writeback_set_ratelimit()`이라는 함수를 통해 결정된다. 시스템의 CPU 수와 메모리 크기에 따라 다를 수 있겠지만 특정 값을 넘어서는 큰 값이 될 경우 보통은 1024로 설정된다. PAGE_CACHE_SIZE의 값은 page의 크기와 같은 4KB이기 때문에 `ratelimit_pages` 값이 1024가 된다면 4MB의 쓰기 작업이 이루어질 때마다 `balance_dirty_pages()` 함수가 호출된다고 볼 수 있다.

`balance_dirty_pages_ratelimited_nr()` 함수를 통과하고 `balance_dirty_pages()`를 성공적으로 호출하게 되면 본격적인 dirty page 크기 확인과 임계치 값을 확인하는 과정이 진행된다.

다음으로 주기적인 동기화를 살펴보자. 이번에는 백그라운드 작업이 영향을 끼치지 않도록 큰 값으로 설정한 후 테스트해보자.

```yaml
root@ip-172-31-0-19:/home/ubuntu# sysctl -w vm.dirty_background_ratio=20
vm.dirty_background_ratio = 20
root@ip-172-31-0-19:/home/ubuntu# sysctl -w vm.dirty_ratio=40
vm.dirty_ratio = 40
root@ip-172-31-0-19:/home/ubuntu# sysctl -w vm.dirty_writeback_centisecs=500
vm.dirty_writeback_centisecs = 500
root@ip-172-31-0-19:/home/ubuntu# sysctl -w vm.dirty_expire_centisecs=1000
vm.dirty_expire_centisecs = 1000
```

위와 같이 설정하면 flush 커널 스레드가 5초에 한 번 깨어나서 생성된 지 10초가 넘은 dirty page들을 동기화하게 된다.

```yaml
Dirty:              7484 kB
Dirty:              8492 kB
Dirty:              9532 kB
Dirty:             10432 kB
Dirty:             11436 kB
Dirty:             12444 kB
Dirty:             13484 kB
Dirty:             14492 kB
Dirty:             15360 kB
Dirty:              1032 kB
Dirty:              2076 kB
Dirty:              3084 kB
Dirty:              4124 kB
Dirty:              5132 kB
Dirty:              6172 kB
Dirty:              7180 kB
Dirty:              8220 kB
Dirty:              9228 kB
Dirty:             10268 kB
Dirty:                 8 kB
```

1초에 1MB씩 쓰기 작업을 하기 때문에 flush 커널 스레드가 깨어나는 타이밍과 맞으면 10~15MB 사이에서 dirty page가 유지된다.

<aside>
💡

flush 커널 스레드가 깨어나는 순간 생성된 지 10초 이상 된 dirty page가 동기화되는데, **동기화 작업은 inode를 기준으로 발생하기 때문에 10초 이상된 dirty page가 포함된 파일의 모든 dirty page가 함께 동기화된다.**

</aside>

flush 커널 스레드가 커널 스케줄러에 의해 깨어나는 것을 확인할 수 있다.

# 10.4 dirty page 설정과 I/O 패턴

```yaml
root@ip-172-31-0-19:/home/ubuntu# ./show_dirty.sh 
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:               148 kB
Dirty:             42124 kB
Dirty:             27280 kB
Dirty:              9456 kB
Dirty:             25704 kB
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:                 0 kB
```

`vm.dirty_background_ratio` 값이 10이기 때문에 8GB 메모리 중 800MB 정도의 dirty page가 생겨야 동기화를 시작한다. (나는 결과랑 좀 다르다.) 그래서 `iostat` 로 I/O 사용량을 살펴보면 대부분 0%에서 갑자기 100%에 이르는 패턴을 보인다. flush 커널 스레드가 깨어나는 조건이 더 길어지는 대신에 한번에 동기화해야 하는 양이 많아지기 때문이다.

```yaml
root@ip-172-31-0-19:/home/ubuntu# ./show_dirty.sh 
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:             50664 kB
Dirty:             50380 kB
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:                 0 kB
Dirty:                 0 kB
```

첫 번째 테스트에 비하면 1/10 수준으로 dirty page의 크기가 유지되는 것을 볼 수 있다. vm.dirty_background_ratio를 10에서 1로 줄였기 때문에 당연한 결과다. (왜 난 결과 다르냐 ㅠㅜㅠ)

더 자주 flush 커널 스레드가 깨어나지만 한 번에 동기화시켜야할 양이 첫번째에 비해 적다. 물론 98%도 높은 수치이긴 하지만 첫 번째 테스트에 비해 io util(%)의 최댓값이 작아졌다. 동기화해야 할 dirty page의 크기가 더 적기 때문이다.

dirty page 동기화와 관련해서 가장 중요한 부분은 flush 커널 스레드를 얼마나 자주 깨울 것인지, 깨울 때 어느 정도의 양을 동기화할지를 설정하는 것이다. 위에서 살펴본 것처럼 자주 깨어나면 io util이 비교적 적지만 flush 커널 스레드가 자주 깨어나는 단점이 있고, 늦게 깨우면 flush 커널 스레드는 자주 깨어나지 않지만 io util이 높아지는 단점이 있다. flush 커널 스레드가 너무 자주 깨어나면 스케줄링에 대한 오버헤드가 발생할 수 있으며, 멀티 스레드 환경의 애플리케이션의 경우 불필요하게 자주 깨어나는 flush 커널 스레드에 cpu 리소스를 빼앗길 수 있기 때문에 성능 저하가 발생할 수 있다. 이렇게 dirty page 동기화를 어떻게 설정하느냐는 각각의 경우에 따라 장단점이 있으며, 어떤 것이 더 낫다는 절대적인 기준은 없다. 따라서 현재 시스템에서 발생하는 워크로드와 디스크의 성능에 따라 결정해야 한다. 예를 들어 똑같이 초당 1MB의 dirty page를 생성하는 애플리케이션이 서로 다른 두 개의 시스템에서 동작하고 있다고 가정해보자. A 시스템의 디스크는 초당 10MB의 쓰기 작업을 견딜 수 있고, B 시스템의 디스크는 초당 100MB의 쓰기 작업을 견딜 수 있다고 했을 때 두 시스템은 같은 애플리케이션을 동작시키지만 dirty page 동기화에 대해서는 서로 다른 전략을 사용해야 한다. A 시스템에서 한 번에 dirty page를 100MB를 동기화하면 background 동기화 속도가 애플리케이션이 생성하는 dirty page 속도를 따라잡지 못할 것이고, 이렇게 되면 결국 dirty_ratio까지 dirty page가 쌓이게 되어 애플리케이션의 성능에 영향을 줄 수 있다. 그렇기 때문에 A 시스템에서는 10MB 단위로 dirty page를 동기화할 수 있도록 설정하는 것이 전체적인 성능에 도움이 된다. 반대로 B 시스템은 디스크의 성능이 좋기 때문에 굳이 10MB 수준에서 flush 커널 스레드를 깨울 필요가 없다.