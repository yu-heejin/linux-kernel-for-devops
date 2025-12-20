# 2.1 시스템의 상태 살피기

```bash
top - 08:12:46 up  1:23,  1 user,  load average: 0.00, 0.00, 0.00
Tasks: 111 total,   1 running, 110 sleeping,   0 stopped,   0 zombie
%Cpu(s):  0.0 us,  0.0 sy,  0.0 ni,100.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st 
MiB Mem :    957.3 total,    212.9 free,    349.6 used,    558.7 buff/cache     
MiB Swap:      0.0 total,      0.0 free,      0.0 used.    607.8 avail Mem 

    PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND                          
    554 root      20   0   17988   8804   7780 S   0.3   0.9   0:00.09 systemd-logind                   
   2358 root      20   0   12372   5728   3552 R   0.3   0.6   0:00.01 top                              
      1 root      20   0   22472  13604   9508 S   0.0   1.4   0:03.97 systemd                          
      2 root      20   0       0      0      0 S   0.0   0.0   0:00.00 kthreadd                         
      3 root      20   0       0      0      0 S   0.0   0.0   0:00.00 pool_workqueue_release           
      4 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R-rcu_gp                 
      5 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R-sync_wq                
      6 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R-kvfree_rcu_reclaim     
      7 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R-slub_flushwq           
      8 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R-netns                  
     10 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/0:0H-events_highpri      
     12 root      20   0       0      0      0 I   0.0   0.0   0:00.10 kworker/u60:0-events_unbound     
     13 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R-mm_percpu_wq           
     14 root      20   0       0      0      0 I   0.0   0.0   0:00.00 rcu_tasks_rude_kthread           
     15 root      20   0       0      0      0 I   0.0   0.0   0:00.00 rcu_tasks_trace_kthread          
     16 root      20   0       0      0      0 S   0.0   0.0   0:00.06 ksoftirqd/0                      
     17 root      20   0       0      0      0 I   0.0   0.0   0:00.14 rcu_sched                        
     18 root      20   0       0      0      0 S   0.0   0.0   0:00.00 rcu_exp_par_gp_kthread_worker/0  
     19 root      20   0       0      0      0 S   0.0   0.0   0:00.00 rcu_exp_gp_kthread_worker        
     20 root      rt   0       0      0      0 S   0.0   0.0   0:00.03 migration/0                      
     21 root     -51   0       0      0      0 S   0.0   0.0   0:00.00 idle_inject/0                    
     22 root      20   0       0      0      0 S   0.0   0.0   0:00.00 cpuhp/0                          
     23 root      20   0       0      0      0 S   0.0   0.0   0:00.00 kdevtmpfs                        
     24 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R-inet_frag_wq           
     25 root      20   0       0      0      0 S   0.0   0.0   0:00.00 kauditd                          
     26 root      20   0       0      0      0 S   0.0   0.0   0:00.00 khungtaskd                       
     27 root      20   0       0      0      0 S   0.0   0.0   0:00.00 oom_reaper                       
     29 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R-writeback              
     30 root      20   0       0      0      0 S   0.0   0.0   0:00.23 kcompactd0                       
     31 root      25   5       0      0      0 S   0.0   0.0   0:00.00 ksmd                             
     32 root      39  19       0      0      0 S   0.0   0.0   0:00.00 khugepaged                       
     33 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R-kintegrityd            
     34 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R-kblockd                
     35 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R-blkcg_punt_bio         
     36 root     -51   0       0      0      0 S   0.0   0.0   0:00.00 irq/9-acpi                       
     37 root      20   0       0      0      0 S   0.0   0.0   0:00.00 xen-balloon 
```

옵션 없이 `top` 명령어를 입력하면 주어진 Interval 간격(기본 3초)으로 화면을 갱신하면서 정보를 보여준다. 만약 순간의 `top` 정보를 확인하고자 한다면 옵션으로 `-b`를 사용한다.

# 2.2 VIRT, RES, SHR…?

프로세스와 관련된 항목 중에 눈에 띄는 것이 있다면 VIRT, RES, SHR을 꼽을 수 있다. 이 세 가지 항목은 현재 프로세스가 사용하고 있는 메모리와 관련된 값이다.

`man` 페이지에는 VIRT를 다음과 같이 정의하고 있다.

*The total amount of virtual memory used by the task. It includes all code, data and shared libraries plus pages that have been swapped out. (작업에서 사용된 가상 메모리의 총 양입니다. 여기에는 스왑된 모든 코드, 데이터 및 공유 라이브러리와 페이지가 포함됩니다.)*

(보통 프로세스라고 부르는) task가 사용하고 있는 virtual memory의 전체 용량이라고 설명한다. 그 다음으로 RES에 대해서는 다음과 같이 정의하고 있다.

*A task’s currently used shared of available physical memory. (작업은 현재 사용 가능한 물리적 메모리를 공유하는 데 사용됩니다.)*

현재 task가 사용하고 있는 물리 메모리의 양을 의미한다고 설명한다. 그럼 마지막으로 SHR은 어떻게 정의하고 있는지 확인해보자.

*The amount of shared memory used by a task. It simply reflects memory that could be potentially shared with other processes. (작업에서 사용하는 공유 메모리의 양입니다. 이는 단순히 다른 프로세스와 공유할 수 있는 메모리를 반영합니다.)*

다른 프로세스와 공유하고 있는 shared memory의 양을 의미한다고 정리한다.

**VIRT는 Task, 즉 프로세스에 할당된 가상 메모리 전체의 크기이며, RES는 그 중 실제로 메모리에 올려서 사용하고 있는 물리 메모리의 크기, 그리고 SHR은 다른 프로세스와 공유하고 있는 메모리의 크기를 말한다.**

SHR의 구체적인 예는 어떤 것이 있을까? 라이브러리가 SHR 영역에 포함될 수 있다. **대부분의 리눅스 프로세스들은 glibc라는 라이브러리를 참조하기 때문에 사용하는 프로세스마다 glibc의 내용을 메모리에 올려서 사용하는 것은 공간 낭비**이다. **커널은 이럴 경우를 대비해서 공유 메모리라는 개념을 도입했고, 다수의 프로세스가 함께 사용하는 라이브러리는 공유 메모리 영역에 올려서 함께 사용하도록 구현했다.**

VIRT는 실제로는 할당되지 않은 가상의 공간이기 떄문에 해당 값이 크다고 해도 문제가 되진 않는다. 실제 사용하고 있는 메모리는 RES 영역이기 때문에 **메모리 점유율이 높은 프로세스를 찾기 위해서는 RES 영역이 높은 프로세스를 찾아야 한다.**

# 2.3 VIRT와 RES 그리고 Memory Commit의 개념

그렇다면 왜 메모리는 VIRT와 RES로 구분되어 있을까? 둘 다 프로세스가 사용하는 메모리를 표현하는 것이지만 차이점이 있다.

VIRT로 표현되는 가상 메모리는 **프로세스가 커널로부터 사용을 예약받은 메모리**라고 생각할 수 있다. 프로세스는 `malloc()`과 같은 시스템 콜로 자신이 필요로 하는 메모리의 영역을 할당해 줄 것을 요청한다. 이에 대해 커널은 가용한 공간이 있다면 성공 메시지와 함께 해당 프로세스가 사용할 수 있도록 가상의 메모리 주소를 전달해준다. 하지만 기억해야 할 것은 **이때에도 물리 메모리에 해당 영역이 할당된 상태는 아니라는 점**이다. 이러한 동작 방식을 Memory Commit 이라고 하며, 커널 파라미터 중에 `vm.overcommit_memory`를 통해서 동작 방식을 정의할 수 있다.

[vm.overcommit에 대한 짧은 이야기](https://brunch.co.kr/@alden/16)

그 후 프로세스가 할당받은 메모리 영역에 실제로 쓰기 작업을 하면 Page fault가 발생하며, 그제서야 커널은 실제 물리 메모리에 프로세스의 가상 메모리 공간을 매핑한다. 이것은 Page Table이라고 불리는 커널의 전역 변수로 관리된다. 그리고 이렇게 물리 메모리에 바인딩된 영역이 RES로 계산된다.

`malloc()`을 이용한 간단한 프로그램을 작성해보자. (practice/malloc_test.c) 코드의 실행 결과는 다음과 같다.

```bash
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   19128   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   20156   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   22212   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   23240   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   24268   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   25296   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   26324   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   27352   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   28380   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   28380   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   29408   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4333 ubuntu    20   0   30436   1640   1512 S   0.0   0.2   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ 

Allocated 1048576 MB
Allocated 2097152 MB
Allocated 3145728 MB
Allocated 4194304 MB
Allocated 5242880 MB
Allocated 6291456 MB
Allocated 7340032 MB
Allocated 8388608 MB
Allocated 9437184 MB
Allocated 10485760 MB
Allocated 11534336 MB
Allocated 12582912 MB
Allocated 13631488 MB
Allocated 14680064 MB
Allocated 15728640 MB
Allocated 16777216 MB
Allocated 17825792 MB
Allocated 18874368 MB
Allocated 19922944 MB
Allocated 20971520 MB
Allocated 22020096 MB
Allocated 23068672 MB
Allocated 24117248 MB
Allocated 25165824 MB
Allocated 26214400 MB
Allocated 27262976 MB
Allocated 28311552 MB
Allocated 29360128 MB
Allocated 30408704 MB
```

프로세스를 살펴보면, 시간이 지나면서 VIRT는 계속 높아지지만 RES는 늘어나지 않는다.

이번엔 주석처리한 부분을 해제하고 프로그램을 돌려보자. 할당받은 메모리를 받은 순간에 바로 사용한다.

```bash
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   21184  20072   1512 S   0.0   2.0   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   23240  22120   1512 S   0.0   2.3   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   24268  23144   1512 S   0.0   2.4   0:00.00 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   25296  24168   1512 S   0.0   2.5   0:00.01 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   26324  25192   1512 S   0.0   2.6   0:00.01 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   26324  25192   1512 S   0.0   2.6   0:00.01 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   27352  26216   1512 S   0.0   2.7   0:00.01 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   28380  27240   1512 S   0.0   2.8   0:00.02 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   29408  28264   1512 S   0.0   2.9   0:00.02 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   30436  29288   1512 S   0.0   3.0   0:00.02 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   31464  30312   1512 S   0.0   3.1   0:00.02 malloc_+
ubuntu@ip-172-31-0-157:~$ top -b -n 1 | grep -i malloc
   4371 ubuntu    20   0   31464  30312   1512 S   0.0   3.1   0:00.02 malloc_+
   
Allocated 1048576 MB
Allocated 2097152 MB
Allocated 3145728 MB
Allocated 4194304 MB
Allocated 5242880 MB
Allocated 6291456 MB
Allocated 7340032 MB
Allocated 8388608 MB
Allocated 9437184 MB
Allocated 10485760 MB
Allocated 11534336 MB
Allocated 12582912 MB
Allocated 13631488 MB
Allocated 14680064 MB
Allocated 15728640 MB
Allocated 16777216 MB
Allocated 17825792 MB
Allocated 18874368 MB
Allocated 19922944 MB
Allocated 20971520 MB
Allocated 22020096 MB
Allocated 23068672 MB
Allocated 24117248 MB
Allocated 25165824 MB
Allocated 26214400 MB
Allocated 27262976 MB
Allocated 28311552 MB
Allocated 29360128 MB
Allocated 30408704 MB
Allocated 31457280 MB
```

아까와는 다르게 RES 영역이 VIRT 영역의 늘어나는 비율과 비슷하게 늘어난다. 그렇기 때문에 메모리 사용과 관련해서 중요한 부분은 VIRT가 아니라 실제로 메모리를 쓰고 있는 RES임을 확인할 수 있다.

[TIL 011 동적 할당 메모리 (malloc lab)](https://velog.io/@chosh/TIL-010-%EB%8F%99%EC%A0%81-%ED%95%A0%EB%8B%B9-%EB%A9%94%EB%AA%A8%EB%A6%AC-malloc-lab)

## 원리

### `malloc()`이 실제로 하는 일

```c
malloc(MEGABYTE);
```

❌ 이 시점에 **물리 메모리를 할당하는 게 아니다**

### 실제 동작

- 커널은:
    - “이 프로세스가 쓸 수 있는 **가상 주소 범위**를 하나 만들어줌”
    - page table에는 **아직 물리 페이지 매핑 없음**
- 이 상태:
    - VIRT ⬆️
    - RES ❌ 그대로

👉 이게 바로 **Lazy Allocation / Demand Paging**

### 2️⃣ 왜 이게 가능한가? → Overcommit + Demand Paging

### Overcommit

```
vm.overcommit_memory

```

보통 기본값:

- `0` 또는 `1`
- “일단 요청은 거의 다 OK, 나중에 실제로 쓰면 그때 판단”

그래서:

```c
malloc(1GB);
malloc(10GB);

```

👉 **성공함**

(아직 안 쓰니까)

### 3️⃣ 그럼 `memset()`은 왜 다를까?

```c
memset(myblock, 1, MEGABYTE);

```

이 한 줄이 **모든 걸 바꿉니다.**

### 이 순간 벌어지는 일

1. CPU가 가상 주소에 **쓰기(write)** 시도
2. 해당 가상 페이지:
    - ❌ 아직 물리 메모리와 매핑 안 됨
3. ➜ **Page Fault 발생**
4. 커널이:
    - 물리 메모리 페이지 하나 확보
    - 가상 주소 ↔ 물리 주소 매핑
    - page table 업데이트
5. 쓰기 재시도 → 성공

👉 결과:

- **RES 증가**
- 실제 RAM 사용 시작

---

## 4️⃣ 왜 VIRT는 둘 다 증가하는가?

| 상황 | VIRT | RES |
| --- | --- | --- |
| `malloc()`만 | ⬆️ | ❌ |
| `malloc()` + `memset()` | ⬆️ | ⬆️ |

이유:

- VIRT = *“프로세스가 사용할 수 있다고 커널이 약속한 주소 공간”*
- RES = *“그중 실제 RAM에 올라간 페이지”*

그렇다면 궁금점이 하나 생긴다. VIRT는 `malloc()` 등의 시스템 콜을 사용하면 늘어나게 되는데, 한도 끝도 없이 늘어나게 될까?

할당받고 사용한 메모리는 RES 영역으로 계산이 되고, 이것은 물리 메모리와 관련이 있기 때문에 더 이상 줄 수 있는 메모리 영역이 없다면 swap을 사용하거나 OOM으로 프로세스를 죽이는 등의 방법으로 메모리를 확보하게 될 것이다.

[OOM의 원인과 아주 간단하게 OOM 발생 시키기](https://cocococo331.tistory.com/17)

그렇다면 VIRT와 같이 실제 사용하지 않는 영역의 경우는 어떻게 될까? 실제로 사용하지 않기 때문에 무한대로 할당을 받을 수 있는 걸까? 대답은 그렇게 할 수도, 그렇게 하지 못하게 막을 수도 있다. 이 동작은 커널 파라미터 중 `vm.overcommit_memory` 파라미터에 의해 결정된다.

좀 더 자세히 알아보기 전에 Memory Commit에 대해 조금 더 이야기 해보자. 프로세스는 자신만의 작업 공간이 필요하고 그 공간은 메모리에 존재한다. **프로세스가 커널에 필요한 만큼의 메모리를 요청하면 커널은 프로세스에 사용 가능한 메모리 영역을 주고 실제로 할당은 하지 않지만 해당 영역을 프로세스에 주었다는 것을 저장해둔다. 이 일련의 과정을 Memory Commit 이라고 부른다.**

그럼 왜 커널은 프로세스의 메모리 요청에 따라 즉시 할당하지 않고 요청을 지연시키는 걸까? 여러가지 이유가 있겠지만 **가장 큰 이유는 `fork()`와 같은, 새로운 프로세스를 만들기 위한 콜을 처리해야 하기 때문이다.** `fork()` 시스템 콜을 사용하면 커널은 현재 실행중인 프로세스와 똑같은 프로세스를 하나 더 만들게 되는데, 대부분은 `fork()` 후 `exec()` 시스템 콜을 통해서 전혀 다른 프로세스로 변한다. 따라서 이때 확보한 메모리 영역이 대부분 쓸모없어질 수도 있다. 그래서 **COW(Copy-On-Write)라는 기법을 통해서 복사된 메모리 영역에 실제 쓰기 작업이 발생한 후에야 실질적인 메모리 할당을 시작한다.** 그리고 이런 작업을 지원하기 위해 Memory Commit이 필요하다. 만약 Memory Commit을 하지 않고 바로 할당한다면 COW와 같은 기술도 사용할 수 없기 때문이다.

[fork() 와 exec()](https://woochan-autobiography.tistory.com/207)

[[OS] 프로세스 생성 - fork(), exec()](https://velog.io/@alirz-pixel/OS-%ED%94%84%EB%A1%9C%EC%84%B8%EC%8A%A4-%EC%83%9D%EC%84%B1-fork)

그렇다면 현재 시스템의 Memory Commit 상태는 어떻게 확인할 수 있을까? `sar` 이라는 모니터링 툴을 이용하면 확인할 수 있다.

```bash
root@ip-172-31-0-157:/home/ubuntu# sar
Linux 6.14.0-1015-aws (ip-172-31-0-157)         12/14/25        _x86_64_        (1 CPU)

00:00:46        CPU     %user     %nice   %system   %iowait    %steal     %idle
00:10:26        all      0.03      0.00      0.03      0.01      0.00     99.92
00:20:46        all      0.02      0.00      0.02      0.00      0.00     99.96
00:30:46        all      0.02      0.00      0.01      0.00      0.00     99.97
00:40:26        all      0.01      0.00      0.01      0.00      0.00     99.97
00:50:46        all      0.01      0.00      0.01      0.00      0.00     99.97
01:00:46        all      0.01      0.00      0.01      0.00      0.00     99.97
01:10:26        all      0.02      0.00      0.01      0.00      0.00     99.96
01:20:46        all      0.01      0.00      0.01      0.00      0.00     99.97
01:30:46        all      0.01      0.00      0.01      0.00      0.00     99.97
01:40:26        all      0.01      0.00      0.01      0.00      0.00     99.97
01:50:46        all      0.01      0.00      0.01      0.00      0.00     99.97
02:00:46        all      0.01      0.00      0.01      0.00      0.00     99.97
02:10:26        all      0.01      0.00      0.01      0.00      0.00     99.98
02:20:46        all      0.01      0.00      0.01      0.00      0.00     99.97
02:30:46        all      0.01      0.00      0.01      0.00      0.00     99.97
02:40:08        all      0.02      0.00      0.02      0.01      0.00     99.95
02:50:46        all      0.01      0.00      0.01      0.00      0.00     99.97
03:00:46        all      0.01      0.00      0.01      0.00      0.00     99.97
03:10:01        all      0.02      0.00      0.02      0.00      0.00     99.95
03:20:46        all      0.01      0.00      0.01      0.00      0.00     99.97
03:30:00        all      0.02      0.00      0.02      0.00      0.00     99.96
03:40:26        all      0.03      0.00      0.02      0.01      0.00     99.94
03:50:46        all      0.01      0.00      0.01      0.00      0.00     99.97
04:00:46        all      0.01      0.00      0.01      0.00      0.00     99.97
04:10:26        all      0.02      0.00      0.01      0.00      0.00     99.97
04:20:46        all      0.01      0.00      0.01      0.00      0.00     99.97
04:30:08        all      0.01      0.00      0.01      0.00      0.00     99.97
04:40:26        all      0.02      0.00      0.01      0.00      0.00     99.96
04:50:16        all      0.01      0.00      0.01      0.00      0.00     99.97
05:00:19        all      0.01      0.00      0.01      0.00      0.00     99.97
05:10:26        all      0.01      0.00      0.01      0.00      0.00     99.97
05:20:46        all      0.02      0.01      0.02      0.02      0.00     99.92

05:20:46        CPU     %user     %nice   %system   %iowait    %steal     %idle
05:30:46        all      0.01      0.00      0.01      0.00      0.00     99.97
05:40:26        all      0.01      0.00      0.01      0.00      0.00     99.97
05:50:46        all      0.01      0.00      0.01      0.00      0.00     99.96
06:00:46        all      0.01      0.00      0.01      0.00      0.00     99.97
06:10:26        all      0.01      0.00      0.01      0.00      0.00     99.97
06:20:46        all      0.02      0.00      0.02      0.00      0.00     99.95
06:30:46        all      0.02      0.00      0.01      0.00      0.00     99.97
06:40:26        all      0.01      0.00      0.01      0.00      0.00     99.98
06:50:46        all      5.29      2.56      3.11      1.53      0.02     87.49
07:00:46        all      0.03      0.00      0.02      0.01      0.00     99.94
07:10:26        all      0.01      0.00      0.01      0.01      0.00     99.96
07:20:46        all      0.89      0.17      0.19      0.16      0.00     98.59
07:30:46        all      0.01      0.00      0.01      0.00      0.00     99.97
07:40:26        all      0.01      0.00      0.01      0.00      0.00     99.98
07:50:46        all      0.01      0.00      0.01      0.00      0.00     99.96
08:00:46        all      0.24      0.00      0.06      0.05      0.00     99.64
08:10:26        all      0.03      0.00      0.02      0.01      0.01     99.93
08:20:46        all      0.01      0.00      0.02      0.00      0.00     99.96
08:30:46        all      0.01      0.00      0.01      0.00      0.00     99.97
08:40:26        all      0.01      0.00      0.01      0.00      0.00     99.97
08:50:46        all      0.01      0.00      0.01      0.00      0.00     99.98
09:00:46        all      0.01      0.00      0.01      0.00      0.00     99.97
09:10:26        all      0.01      0.00      0.01      0.00      0.00     99.98
09:20:46        all      0.03      0.00      0.02      0.00      0.00     99.94
09:30:46        all      0.01      0.00      0.01      0.00      0.00     99.98
09:40:00        all      0.09      0.00      0.02      0.06      0.00     99.83
09:50:46        all      0.02      0.00      0.04      0.01      0.00     99.93
10:00:46        all      0.01      0.00      0.01      0.00      0.00     99.97
10:10:26        all      0.01      0.00      0.01      0.00      0.00     99.98
10:20:46        all      0.01      0.00      0.01      0.00      0.00     99.97
10:30:46        all      0.01      0.00      0.01      0.00      0.00     99.98
10:40:26        all      0.03      0.00      0.02      0.00      0.00     99.94

10:40:26        CPU     %user     %nice   %system   %iowait    %steal     %idle
10:50:46        all      0.01      0.00      0.01      0.00      0.00     99.97
11:00:46        all      0.02      0.00      0.01      0.01      0.00     99.97
11:10:46        all      0.01      0.00      0.01      0.00      0.00     99.97
11:20:46        all      0.03      0.00      0.02      0.01      0.00     99.94
11:30:46        all      0.01      0.00      0.01      0.00      0.00     99.98
11:40:26        all      0.01      0.00      0.01      0.00      0.00     99.98
11:50:46        all      0.01      0.00      0.01      0.00      0.00     99.98
12:00:39        all      0.02      0.00      0.01      0.01      0.00     99.96
Average:        all      0.11      0.04      0.06      0.03      0.00     99.76
root@ip-172-31-0-157:/home/ubuntu# 
```

`%commit`의 숫자는 시스템의 메모리 커밋 비율을 나타낸다. 할당만 해주고 실제 사용하지 않는 메모리의 양이 전체 메모리의 3.00% 정도라는 것이다. 저 메모리에 순간적으로 쓰기 작업이 들어가도 시스템 전체적으로는 전혀 문제가 없는 상황이다.

하지만 커밋된 메모리의 비율이 높다면 순간적으로 시스템에 부하를 일으키거나 최악의 경우에는 커널 응답 불가 현상을 일으킬 수도 있다. 그렇기 때문에 커널은 메모리 커밋에 대한 동작 방식을 `vm.overcommit_memory`라는 파라미터로 제어할 수 있게 해두었다. 이 값은 0, 1, 2 세 가지로 세팅할 수 있는데, 각각의 값이 가지는 의미는 다음과 같다.

- 0: 커널에서 사용하고 있는 기본 값이다. 별다른 세팅을 하지 않았다면 0으로 설정되며, overcommit할 수 있는 최댓값은 page cache와 swap 영역 그리고 slab reclaimable(slap 메모리 중에서 필요한 경우 다시 회수할 수 있는 메모리의 양) 세 가지의 값을 합한 값이 된다. 현재 메모리에 가용 공간이 얼마인지는 고려하지 않는다. 저기서 합산된 값보다 작으면 계속해서 commit 할 수 있게 된다.

[top 명령어 제대로 알고 쓰자](https://breeze-winter.tistory.com/49)

[[Linux / Memory] 메모리의 Slab 영역과 slabtop 명령어, slab 할당자(allocator)](https://youngswooyoung.tistory.com/65)

- 1: 무조건 commit을 진행한다. 아무것도 계산하지 않고 요청 온 모든 메모리에 대해 commit이 일어난다. 이렇게 되면 가용 메모리보다 훨씬 큰 메모리를 요청하는 일이 발생할 수 있다. 메모리 누수가 있는 프로세스가 있다면 시스템 응답 불가 현상을 일으킬 수도 있다.
- 2: 제한적으로 commit을 진행한다. 값이 0일때와 같이 계산식이 있으며, `vm.overcommit_ratio` 에 설정된 비율과 swap 영역의 크기를 토대로 계산된다. 이 값은 `/proc/meminfo` 에서도 확인할 수 있다.

커널은 어찌되었든 overcommit을 지원한다. 다만 overcommit할 수 있는 최댓값을 결정하는 방법을 세 가지로 나누어서 관리한다. 0, 2번에서도 본 것처럼 swap 영역은 commit 메모리를 결정하는 데 큰 역할을 한다. 즉, 시스템의 안정성을 유지하는 데에 큰 역할을 한다고 볼 수 있다.

이렇게 `vm.overcommit_memory` 파라미터를 이용해서 커널의 메모리 커밋 동작을 제어할 수 있으며 프로세스에게 할당되는 VIRT 메모리의 양도 제어할 수 있다.

# 2.4 프로세스의 상태 보기

프로세스의 상태는 SHR 옆에 있는 S 항목으로 볼 수 있다.

```bash
S  --  Process Status
           The status of the task which can be one of:
               D = uninterruptible sleep
               I = idle
               R = running
               S = sleeping
               T = stopped by job control signal
               t = stopped by debugger during trace
               Z = zombie
```

- D: uninterruptible sleep 상태로, 디스크 혹은 네트워크 I/O를 대기하고 있는 프로세스를 의미한다. 이 상태의 프로세스들은 대기하는 동안 Run Queue에서 빠져나와 Wait Queue에 들어가게 된다.
    
    [Context Switching, Run Queue, Wait Queue, Scheduling, Multi-Tasking](https://jj-0.tistory.com/6)
    
- R: 실행중인 프로세스를 의미한다. 실제로 CPU 자원을 소모하고 있는 프로세스이다.
- S: sleeping 상태의 프로세스로, D 상태와의 가장 큰 차이점은 요청한 리소스를 즉시 사용할 수 있는지 여부이다. (즉시 사용 가능)
- T: traced or stopped 상태의 프로세스로, strace 등으로 프로세스의 시스템 콜을 추적하고 있는 상태를 보여준다. 보통의 시스템에서는 자주 볼 수 없는 상태의 프로세스이다.
- Z: zombie 상태의 프로세스이다. 부모 프로세스가 죽은 자식 프로세스를 의미한다.

먼저 Uninterruptible sleep 상태에 대해 알아보자. 프로세스가 디스크 혹은 네트워크 작업을 하게 되면 디스크 디바이스 혹은 네트워크 디바이스에 요청을 보낸다. 디스크를 예로 든다면 어느 블록에 있는 어느 데이터를 읽어 달라고 요청하는 것이다. **프로세스의 입장에서 보면 보낸 요청이 도착할 때까지 아무것도 할 수 없기 때문에, CPU에 대한 사용권을 다른 프로세스에 넘기고 자신을 UNINTERRUPTIBLE 상태로 마킹한 후 대기 상태로 빠진다.** 이렇게 요청 후에 그에 대한 응답을 기다려야 하는 상태를 Uninterruptible sleep 상태, 즉 D 상태라고 말할 수 있다.

[[Linux] Uninterruptible sleep 프로세스 상태 D](https://hotpotato.tistory.com/280)

반면에 `sleep()` 시스템 콜 등을 호출해서 타이머를 작동시키거나, 콘솔 입력을 기다리는 프로세스들은 Interruptible sleep 상태가 된다. 이 상태는 특정 요청에 대한 응답을 기다리는 상태가 아니며, **언제 어떻게 시그널이 들어올지 모르기 때문에 언제든 시그널을 받아서 처리할 수 있도록 INTERRUPTIBLE 상태로 마킹하고 대기 상태에 빠진다.** 이때의 상태를 S 상태라고 한다.

[What Is an Uninterruptible Process in Linux? | Baeldung on Linux](https://www.baeldung.com/linux/uninterruptible-process)

S 상태의 프로세스가 많은 것은 시스템에 큰 영향을 끼치지 않는다. 하지만 D 상태의 프로세스가 많으면 특정 요청이 끝나기를 기다리고 있는 프로세스가 많다는 뜻이고, 이 프로세스들은 요청이 끝나면 R 상태로 다시 돌아가야 하기 때문에 시스템의 부하를 계산하는 데 포함된다.

그렇다면 Z 상태는 어떤 경우에 발생할까? 모든 프로세스는 `fork()`를 통해서 만들어지기 때문에 부모와 자식 관계가 되고, 보통 부모 프로세스는 자식이 완료될 때까지 기다리게 된다. 하지만 그렇지 못한 경우, 즉 부모 프로세스가 죽었는데도 자식 프로세스가 남아 있거나 자식 프로세스가 죽기 전에 비정상적인 동작으로 부모 프로세스가 죽는 경우가 발생할 수 있다. 이런 경우 좀비 프로세스가 만들어진다.

좀비 프로세스는 시스템의 리소스를 차지하지 않기 때문에 그 존재 자체는 큰 문제가 되지 않는다. 스케줄러에 의해 선택되지 않기 때문에 당연히 CPU를 사용하지 않고, 좀비 프로세스 자체는 이미 사용이 중지된 프로세스이기 때문에 메모리를 사용하지도 않는다. 그런데 왜 문제가 될까? 바로 좀비 프로세스가 점유하고 있는 PID 때문이다.

요점부터 말하면 **좀비 프로세스는 “이미 실행이 끝난 프로세스”이기 때문에 리소스를 거의 안 먹습니다.**

조금 더 정확히 풀면 아래 구조 때문입니다.

---

## 1️⃣ 좀비 프로세스의 정체

좀비 프로세스(Zombie process)는

- **실행은 이미 종료됨 (`exit() 호출 완료`)**
- 하지만 **부모 프로세스가 `wait()` / `waitpid()`로 종료 상태를 수거하지 않은 상태**

입니다.

즉,

> 프로세스의 “시체”만 커널에 남아 있는 상태
> 

---

## 2️⃣ 이미 해제된 것 vs 남아 있는 것

### ❌ 이미 해제된 리소스

프로세스가 `exit()` 하면 커널은 즉시 아래 리소스를 해제합니다.

- 사용자 공간 메모리 (heap, stack, code 영역)
- 열린 파일 디스크립터
- 소켓
- 락(lock)
- CPU 스케줄링 대상

➡ **CPU 사용 0%**

➡ **메모리 사용 0 (가상 주소 공간 없음)**

---

### ✅ 좀비가 들고 있는 유일한 것

커널에 **아주 작은 메타데이터만 남습니다.**

- PID
- 종료 코드 (`exit status`)
- 자원 사용 통계 (`rusage`)
- 부모 PID

이 정보는 **부모 프로세스가 `wait()`로 읽어가야 하기 때문에 유지됨**

➡ 커널의 **프로세스 테이블 엔트리 1개**

---

## 3️⃣ 왜 CPU / 메모리를 안 먹는가?

### 🔹 CPU

- 스케줄러 관점에서 **실행 가능한 프로세스가 아님**
- `TASK_ZOMBIE` 상태
- Run Queue에 들어가지 않음

➡ CPU 스케줄링 대상 ❌

---

### 🔹 메모리

- 사용자 메모리 공간은 전부 해제됨
- 커널이 들고 있는 정보는 **수십~수백 바이트 수준**

➡ 일반적인 메모리 사용량에 거의 영향 없음

---

## 4️⃣ 그럼 왜 문제냐?

리소스를 “먹지는 않지만” **무한히 쌓이면 문제**

- PID 고갈
- 프로세스 테이블 엔트리 고갈
- `fork()` 실패 (`EAGAIN`)

➡ **좀비가 많으면 시스템이 새 프로세스를 못 만듦**

---

## 5️⃣ 한 줄 요약 (면접용)

> 좀비 프로세스는 실행이 이미 끝난 상태라 CPU나 메모리를 사용하지 않고,
부모가 종료 상태를 수거할 수 있도록 PID와 종료 코드 같은 최소한의 커널 메타데이터만 남아 있기 때문이다.
> 

---

원하면 다음도 이어서 설명 가능해:

- orphan 프로세스와의 차이
- `init(1)`이 좀비를 정리하는 이유
- `SIGCHLD` 처리 방식
- 실무에서 좀비 생기는 패턴 (fork + wait 누락)

**좀비 프로세스가 사용한 PID가 정리되지 않고 쌓이면 새로운 프로세스에 할당할 PID가 모자라게 되고, 이는 결국 더 이상 PID를 할당하지 못하는 PID 고갈을 일으킬 수 있다.**

### 생성 가능한 최대 PID 확인하기

리눅스 상의 모든 프로세스들은 PID를 가지며, 이 PID는 Integer 형태의 숫자로 구성되어 있다. 그리고 리눅스 커널에는 `kernel.pid_max` 라는 커널 파라미터를 통해서 PID의 최댓값을 정의하고 있다.

```bash
ubuntu@ip-172-31-0-157:~$ sysctl -a | grep -i pid_max
kernel.pid_max = 4194304
```

이 시스템에서 생성되는 프로세스가 가질 수 있는 PID의 최댓값은 4194304이다. 그래서 이 시스템에서 생성되는 모든 프로세스는 1~4194304 사이의 임의의 값을 PID로 배정받는다. 또한 최댓값이 4194304이기 때문에 프로세스 개수도 4194304개 이상 존재할 수 없다.

# 2.5 프로세스의 우선순위

마지막으로 `top`을 통해 볼 수 있는 항목 중 PR과 NI 값을 살펴보자. PR과 NI는 커널이 프로세스를 스케줄링할 때 사용하는 우선순위를 나타내는 값이다.

CPU마다 Run Queue라는 것이 존재하며, Run Queue에는 우선순위별로 프로세스가 연결되어 있다.

스케줄러는 유휴 상태에 있던 프로세스가 깨어나거나 특정 프로세스가 스케줄링을 양보하는 등의 경우에 현재 Run Queue에 있는 프로세스들 중 가장 우선순위가 높은 프로세스를 꺼내서 디스패처에 넘겨준다. 디스패처는 현재 실행중인 프로세스의 정보를 다른 곳에 저장한 후 넘겨받은 프로세스의 정보를 가지고 다시 연산하도록 요청한다.

PR이 어떤 의미인지 살펴보자. `man top` 을 통해 확인한 값은 다음과 같다.

```bash
PR  --  Priority
           The scheduling priority of the task.  If you see `rt' in this field, it means the task is running under real time scheduling priority.
           
           PR (Priority)
  해당 작업(task)의 스케줄링 우선순위를 의미한다.
  이 필드에 rt라고 표시되어 있다면, 해당 작업이 실시간(real-time) 스케줄링 우선순위로 실행 중임을 의미한다.
```

이 값은 커널에서 인식하는 해당 프로세스의 실제 우선순위 값을 의미한다.

NI는 다음과 같이 설명하고 있다.

```bash
NI  --  Nice Value
       The nice value of the task.  A negative nice value means higher priority, whereas a positive nice value means lower priority.  Zero in this field simply means priority will not  be  adjusted  in
       determining a task's dispatch-ability.
       
NI (Nice Value)
  해당 작업(task)의 nice 값을 의미한다.
  음수의 nice 값은 더 높은 우선순위를 뜻하고, 양수의 nice 값은 더 낮은 우선순위를 의미한다.
  이 필드의 값이 0이면, 작업의 디스패치(스케줄링) 가능성을 결정하는 과정에서 우선순위가 추가로 조정되지 않음을 의미한다.
```


NI는 nice 값이라고 부르며, 명령어를 통해서 우선순위를 낮출 때 사용된다. 우선순위는 값이 낮을수록 우선순위가 높은 것으로, `nice` 명령을 이용해서 PR 값을 낮출 수 있다. 그럼 더 높은 우선순위를 가지기 때문에 더 많은 스케줄링이 될 수 있는 기회를 얻게 된다.

기본적으로 모든 프로세스들은 20의 우선순위 값을 갖는데 여기에 `nice` 값을 주면 우선순위 값이 바뀐다. 제일 밑에 있는 `auditd` 데몬처럼 `nice` 값이 -4가 되면 20에 -4가 적용된 16이 되며 20보다 더 자주 실행된다.

`nice`로 우선순위를 낮춘 프로세스가 먼저 끝나지 않는다. 얼핏 생각하기에는 먼저 끝나는게 맞는데 먼저 끝날 때도 있고 더 늦게 끝날 때도 있다. 그 이유는 바로 CPU Core 수와 관련이 있다. CPU Core 수가 만약 2개라면, A 프로세스 실행 중에 우선순위가 낮은 B 프로세스가 실행되어도 나뉘어서 실행되기 때문에 nice의 효과가 없다. `nice`로 우선순위를 낮췄다고는 해도 CPU Core 수(테스트용 시스템은 CPU Core가 2개 있다)와 동일한 수의 프로세스가 돌아가고 있다면 CPU 경합을 벌일 필요가 없기 때문에 `nice`로 우선순위를 낮춰도 비슷한 시간에 끝나게 된다.

이번에는 터미널을 하나 더 열어서 3개의 프로세스를 동시에 실행시켜보자. 이번엔 첫 번째 터미널에서 `nice`로 우선순위를 낮춘 프로세스가 확실하게 먼저 끝난다.

그림 2-13을 보면 `nice` 값으로 우선순위를 낮춘 프로세스가 CPU 1개를 독차지하고 나머지 두 개가 하나의 CPU를 나눠서 사용한다. 그래서 무조건 우선순위를 낮춘 프로세스가 먼저 끝나게 된다. CPU Core가 2개일 때 동일한 우선순위의 A, B 프로세스 실행중에 우선순위가 더 낮은 C 프로세스가 생성되면 A 프로세스는 다른 CPU의 Run Queue로 옮겨간다.

동작중인 프로세스가 빨리 실행되어야 한다면 `renice` 명령을 통해 우선순위를 낮출 수 있다. 다만, 그 경우에도 CPU Core 수와 프로세스의 수가 동일하다면 효과를 볼 수 없기 때문에 잘 계산해서 우선순위를 조절해야 한다.

그 외에 우선순위와 관련된 출력 값을 보면 재밌는 값이 하나 보이는데 바로 RT로 표현되는 프로세스들이다. 사실 RT(RealTime) 스케줄러는 일반적인 사용자가 생성한 프로세스에 적용되는 스케줄러가 아니다. 이름에서도 알 수 있듯이 반드시 특정 시간 안에 종료되어야 하는 중요한 프로세스들, 바로 커널에서 사용하는 데몬들이 대상이다.

[116. [RTOS] 1편 : Real-time 개념 및 리눅스에서의 RT Scheduling 이해하기](https://m.blog.naver.com/PostView.naver?blogId=alice_k106&logNo=221149061940&proxyReferer=https:%2F%2Fwww.google.com%2F&trackingCode=external)

RT 스케줄러의 적용을 받게되는 프로세스들은 CFS(Completely Fair Scheduling) 스케줄러보다 더 먼저 실행된다. 이는 시스템의 안정적인 운영을 위한 것이다. 메모리가 부족한 상태에서 사용하지 않는 메모리들을 회수하고 관리하는 프로세스가 일반적인 사용자 프로세스보다 더 중요함은 따로 설명할 필요가 없을 정도로 자명한 일이다.