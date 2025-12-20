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

/proc 파일 시스템과 관련된 커널 소스는 fs/proc/에 위치해 있다. 그중 loadavg 파일과 관련된 파일은 fs/proc/loadavg.c 파일이다. 이 파일을 보면 `loadavg_proc_show()` 함수를 볼 수 있다.