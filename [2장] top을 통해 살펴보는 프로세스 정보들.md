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