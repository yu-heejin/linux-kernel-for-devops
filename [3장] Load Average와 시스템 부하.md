# 3.1 Load Average의 정의

`man proc` 을 통해 loadavg를 찾아보면 다음과 같이 정의하고 있다.

```bash
The first three fields in this file are load average figures giving the number of
jobs in the run queue (state R) or waiting for disk I/O(state D) averaged over
1, 5, and 15 minutes

이 파일의 처음 세 개 필드는 로드 평균(load average) 값을 나타내며,
실행 큐(run queue)에 있는 작업(R 상태) 또는
**디스크 I/O를 기다리고 있는 작업(D 상태)**의 수를
1분, 5분, 15분 동안 평균낸 값을 의미한다.
```

2장에서 확인한 프로세스의 상태의 상태 중 R과 D 상태에 있는 프로세스 개수의 1분, 5분, 15분 마다의 평균 값이라고 설명해 놓았다. 즉, 얼마나 많은 프로세스가 실행중 혹은 실행 대기중이냐를 의미하는 수치이다. **Load Average가 높다면 많은 수의 프로세스가 실행중이거나 I/O 등을 처리하기 위한 대기 상태에 있다는 것이며, 낮다면 적은 수의 프로세스가 실행중이거나 대기중이라는 의미이다.** 프로세스의 수를 세는 것이기 때문에 시스템에 있는 **CPU Core 수가 몇개냐에 따라 각각의 값은 의미가 상대적이다.**

CPU Core가 하나인 경우와 2개인 경우 둘 다 Load Average 값은 2의 근사값이 나오겠지만 그 의미는 좀 다르다. 첫번째 경우는 하나의 Run Queue에 두 개의 프로세스가 있으며, 이 경우 한 번에 하나만 실행이 되기 때문에 나머지 하나의 프로세스는 대기 상태에 있을 수밖에 없다. 이는 현재 시스템이 처리할 수 있는 프로세스보다 조금 더 많은 프로세스가 있다는 뜻이다. 하지만 두번째의 경우는 첫번째와 똑같이 Run Queue에 두 개의 프로세스가 있지만 서로 다른 CPU에 있기 때문에 A와 B는 동시에 실행될 수 있다. 현재 시스템에 처리 가능한 만큼의 프로세스가 있는 것이다. 이처럼 같은 Load Average라고 해도 CPU Core가 몇 개냐에 따라 전혀 다른 의미일 수 있다.

# 3.2 Load Average 계산 과정

이번에는 Load Average가 커널 내부에서 어떻게 계산되는지 살펴보자. 바로 `uptime` 명령이다.

```bash
ubuntu@ip-172-31-0-157:~$ uptime
 12:53:51 up 1 day, 21:58,  1 user,  load average: 0.00, 0.00, 0.00
```

우선 `strace`를 이용해서 `uptime` 명령을 분석해보자.

```bash
ubuntu@ip-172-31-0-157:~$ strace -s 65535 -f -t -o uptime_dump uptime
 12:54:52 up 1 day, 21:59,  1 user,  load average: 0.00, 0.00, 0.00
ubuntu@ip-172-31-0-157:~$ ls
malloc_test  malloc_test.c  nice_test.py  uptime_dump
```

생성된 덤프 파일을 편집기로 열어서 천천히 살펴볼 것이다. `execve()`를 통해서 `bash`가 `uptime` 명령을 실행시키고, 관련된 라이브러리 파일들을 읽는 과정을 확인할 수 있다. 그러다가 덤프 파일 하단부에 보면 아래와 같은 내용이 보인다.

```bash
9114  12:54:52 openat(AT_FDCWD, "/proc/loadavg", O_RDONLY) = 3
9114  12:54:52 fstat(3, {st_mode=S_IFREG|0444, st_size=0, ...}) = 0
9114  12:54:52 read(3, "0.00 0.00 0.00 1/143 9114\n", 1024) = 26
9114  12:54:52 close(3)                 = 0
9114  12:54:52 fstat(1, {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0x1), ...}) = 0
9114  12:54:52 write(1, " 12:54:52 up 1 day, 21:59,  1 user,  load average: 0.00, 0.00, 0.00\n", 68) = 66
```

`uptime` 명령은 /proc/loadavg 파일을 열어서 그 파일의 내용을 읽고 화면에 출력해주는 명령이다. 즉, 직접 Load Average 값을 계산하는게 아니고 커널이 미리 준비해둔 /proc/loadavg 를 단순히 읽어서 보여주는 명령이다. 그럼 이번엔 /proc/loadavg를 열어보자.

```bash
ubuntu@ip-172-31-0-157:~$ cat /proc/loadavg
0.00 0.00 0.00 3/140 9134
```

예상대로 `uptime`에서 본 값들이 기록되어 있다. 그럼 이 값은 어떻게 만들어지는 걸까? 이제 커널 코드를 살펴볼 차례다.

여기서 중요한 두 가지 사실을 알 수 있는데, `active` 변수와 `calc_load()` 함수이다. 먼저 `active` 변수의 값을 살펴보자. `active` 변수의 값에 대해 알기 위해서는 `calc_load_tasks`가 어떤 값을 가지게 되는지 살펴봐야 한다.

1. `nr_active` 변수에 Run Queue를 기준으로 nr_running 상태의 프로세스 개수를 입력한다. 이 프로세스들이 바로 R 상태의 프로세스다.
2. `nr_active` 변수에 Run Queue를 기준으로 nr_uninterruptible 상태의 프로세스 개수를 더해준다. 이 프로세스들이 바로 D 상태의 프로세스다.
3. `nr_active` 값이 기존에 계산된 값과 다르다면 그 차이 값을 구한 후 `calc_load_tasks` 변수에 입력한다.

이렇게 `cpu_load_account_active()` 함수가 매번 Tick 주기마다 깨어나서 현재 CPU의 Run Queue에 있는 nr_running 프로세스의 개수와 nr_uninterruptible 프로세스의 개수를 세어서 `calc_load_tasks` 변수에 넣어준다.

[](https://blog.naver.com/sigsaly/220555533989)

그 후 5초 간격으로 `calc_global_load()` 함수가 `calc_load_tasks` 변수 값을 바탕으로 1분, 5분, 15분 마다의 평균 Load Average를 계산해서 넣어준다.

그림 3-2를 보면 커널 타이머가 두 함수를 주기적으로 호출한다. 먼저 `calc_load_account_active()`가 더 잦은 빈도로 호출되며 그때마다 `calc_load_tasks`의 변수를 갱신한다. 그 후 `calc_global_load()` 함수가 호출되어 내부적으로 active 변수에 `calc_load_tasks`의 값을 저장하고 `calc_load()` 함수를 통해서 최종적으로 계산된 값을 `avenrun[]` 배열에 저장한다. 과정이 조금 복잡해 보일 수 있지만, 결국 프로세스의 개수를 센다는 점만 기억하면 된다.

# 3.3 CPU Bound vs I/O Bound

지금까지 Load Average가 계산되는 과정을 살펴봤다. 결국 Load Average는 상대적인 값이 아니고 계산하는 순간을 기준으로 존재하는 nr_running 상태의 프로세스 개수와 nr_uninterruptible 상태의 프로세스 개수를 합한 값을 바탕으로 계산되는 것이었다.

Load Average가 높다는 것은 단순히 CPU를 사용하려는 프로세스가 많다는 것을 의미하는 것이 아니고, I/O에 병목이 생겨서 I/O 작업을 대기하는 프로세스가 많을 수도 있다는 의미이다. Load Average 값만으로는 시스템에 어떤 상태의 부하가 일어나는지 확인하기 어렵다는 뜻이기도 하다.

부하를 일으키는 프로세스는 크게 두 가지 종류로 나눌 수 있다. nr_running으로 표현되는, **CPU 자원을 많이 필요로 하는 CPU Bound 프로세스**와 nr_uninterruptible로 표현되는, **많은 I/O 자원을 필요로 하는 I/O Bound 프로세스**이다.

첫번째는 무한루프를 돌면서 수치 연산을 하는 파이썬 스크립트이다. 이 스크립트를 실행시켜서 `uptime` 명령을 통해서 확인해보면 Load Average가 올라가는 것을 확인할 수 있다.

```c
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:37 up 30 min,  2 users,  load average: 0.30, 0.07, 0.02
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:38 up 30 min,  2 users,  load average: 0.36, 0.08, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:39 up 30 min,  2 users,  load average: 0.36, 0.08, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:40 up 30 min,  2 users,  load average: 0.36, 0.08, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:41 up 30 min,  2 users,  load average: 0.36, 0.08, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:42 up 30 min,  2 users,  load average: 0.36, 0.08, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:43 up 31 min,  2 users,  load average: 0.36, 0.08, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:43 up 31 min,  2 users,  load average: 0.41, 0.10, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:44 up 31 min,  2 users,  load average: 0.41, 0.10, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:45 up 31 min,  2 users,  load average: 0.41, 0.10, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:46 up 31 min,  2 users,  load average: 0.41, 0.10, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:46 up 31 min,  2 users,  load average: 0.41, 0.10, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:47 up 31 min,  2 users,  load average: 0.41, 0.10, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:47 up 31 min,  2 users,  load average: 0.41, 0.10, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:47 up 31 min,  2 users,  load average: 0.41, 0.10, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:48 up 31 min,  2 users,  load average: 0.41, 0.10, 0.03
ubuntu@ip-172-31-0-157:~$ uptime
 14:50:48 up 31 min,  2 users,  load average: 0.46, 0.11, 0.04
```

이번에는 무한루프를 돌면서 I/O를 발생시키는 파이썬 스크립트를 실행시켜 보자. 이 스크립트를 실행시키면 첫번째 예제와 마찬가지로 Load Average가 올라가는 것을 확인할 수 있다.

```c
ubuntu@ip-172-31-0-157:~$ uptime
 14:53:27 up 33 min,  2 users,  load average: 0.20, 0.11, 0.04
ubuntu@ip-172-31-0-157:~$ uptime
 14:53:28 up 33 min,  2 users,  load average: 0.20, 0.11, 0.04
ubuntu@ip-172-31-0-157:~$ uptime
 14:53:28 up 33 min,  2 users,  load average: 0.26, 0.12, 0.04
ubuntu@ip-172-31-0-157:~$ uptime
 14:53:29 up 33 min,  2 users,  load average: 0.26, 0.12, 0.04
ubuntu@ip-172-31-0-157:~$ uptime
 14:53:30 up 33 min,  2 users,  load average: 0.26, 0.12, 0.04
ubuntu@ip-172-31-0-157:~$ uptime
 14:53:31 up 33 min,  2 users,  load average: 0.26, 0.12, 0.04
ubuntu@ip-172-31-0-157:~$ uptime
 14:53:32 up 33 min,  2 users,  load average: 0.26, 0.12, 0.04
ubuntu@ip-172-31-0-157:~$ uptime
 14:53:35 up 33 min,  2 users,  load average: 0.32, 0.14, 0.05
ubuntu@ip-172-31-0-157:~$ uptime
 14:53:37 up 33 min,  2 users,  load average: 0.32, 0.14, 0.05
ubuntu@ip-172-31-0-157:~$ uptime
 14:53:41 up 33 min,  2 users,  load average: 0.38, 0.15, 0.06
```