메모리는 커널이 제공하는 중요한 리소스 중 하나이다. CPU가 프로세스의 연산 과정에 필요한 리소스라고 한다면, **메모리는 프로세스가 연산할 수 있는 공간을 제공해주는 리소스라고 할 수 있다.** 프로세스는 메모리라는 공간에 자신이 필요한 함수를 넣어두거나 변수에 값을 저장하거나 하는 방식으로 연산을 위한 공간을 확보하고 작업을 진행한다. 메모리가 부족하다면 프로세스는 더 이상 연산을 위한 공간을 확보할 수 없고, 이는 시스템 응답 불가 현상 또는 큰 성능 저하를 일으킬 수 있다. 그렇기 때문에 메모리가 어떻게 활용되고 있는지를 파악하는 것은 CPU 사용률과 Load Average 만큼 중요한 포인트이다.

# 4.1 메모리 사용량 확인하기

리눅스 시스템에서 메모리의 전체적인 현황을 가장 빠르게 살펴볼 수 있는 명령은 `free` 명령이다. `free` 명령은 전체 메모리 용량과, 사용 중인 용량, 그리고 buffers와 cached로 명명되는 캐싱 영역의 용량을 확인하는데 사용된다.

아래는 `free` 명령의 출력 결과이다.

```c
ubuntu@ip-172-31-0-119:~$ free -m
               total        used        free      shared  buff/cache   available
Mem:             957         318         154           0         642         638
Swap:              0           0           0
```

- Mem
    - total: 현재 시스템에 설치되어 있는 전체 메모리 양을 의미한다. `-m`을 옵션으로 주었기 때문에 MB 단위로 표시한다.
    - used: 시스템에서 사용하고 있는 메모리 양을 의미한다. 이 시스템은 957MB 중에서 318MB를 사용하고 있다.
    - free: 시스템에서 아직 사용하고 있지 않는 메모리 양을 의미한다. 그야말로 아무도 사용하고 있지 않기 때문에 애플리케이션이 사용할 수도 있고, 커널이 사용할 수도 있다.
    - shared: 프로세스 사이에 공유하고 있는 메모리 양이다. 출력에는 MB 단위라서 0으로 출력되었지만 단위를 더 낮추면 값이 보일 수 있다.
    - buffers: 버퍼 용도로 사용하고 있는 메모리 양을 의미한다. 프로세스가 사용하는 메모리 영역이 아니고 시스템의 성능 향상을 위해서 커널에서 사용하고 있는 영역이다.
    - cached: 페이지 캐시라고 불리는 캐시 영역에 있는 메모리 양을 의미한다. I/O 관련 작업을 더 빠르게 진행하기 위해 커널에서 사용하고 있는 영역이다.
- buffers/cache(buff/cache): 첫번째 줄의 결과에서 buffers와 cached 영역을 제외한 양을 보여준다는 의미이다. buffers와 cached 영역을 제외하고 사용하고 있는 영역을 의미한다.
    - free: buffers와 cache영역을 제외하고 사용하지 않는 영역을 의미한다. 첫번째 줄에서 볼 수 있는 free 영역보다 더 큰 값을 볼 수 있다.
- Swap
    - total: swap 영역의 전체 용량을 보여준다.
    - used: swap 영역 중 실제로 사용하고 있는 영역에 대한 정보이다.
    - free: swap 영역 중 사용하지 않은 영역에 대한 정보이다.

# 4.2 buffers와 cached 영역

커널은 블록 디바이스라고 부르는 디스크로부터 데이터를 읽거나 사용자의 데이터를 디스크에 저장한다. 하지만 디스크는 다른 장치들에 비해 매우 느리기 때문에 디스크에 대한 요청을 기다리는 시간이 상당히 많이 소요되고, 이로 인해 시스템에 부하가 일어나기도 한다. **커널은 이렇게 상대적으로 느린 디스크에 대한 요청을 좀 더 빠르게 하기 위해 메모리의 일부를 디스크 요청에 대한 캐싱 영역으로 할당해서 사용한다. 즉, 한번 읽은 디스크의 내용을 메모리에 저장해 두어서, 동일한 내용을 읽고자 하면 디스크로 요청하지 않고 메모리로 요청하게 된다.** 이런 캐싱 기능을 통해서 커널은 다수의 디스크 요청을 좀 더 빠르게 처리할 수 있다. 그리고 이때 사용되는 캐싱 영역을 buffers, cached라고 부른다.

커널은 블록 디바이스에서 데이터를 읽을 때 데이터가 위치한 특정 블록의 주소를 넘겨주고, 블록 디바이스는 해당 블록 주소의 데이터를 커널에 전달한다. 이 과정에 디바이스 드라이버가 있다. 커널이 읽어야 할 데이터가 파일의 내용이라면 커널은 bio 구조체를 만들고 해당 구조체에 Page Cache 용도로 할당한 메모리 영역을 연결해준다. 그리고 bio 구조체는 디바이스 드라이버와 통신해서 디스크로부터 데이터를 읽어서 Page Cache에 파일의 내용을 채운다.

[[Block I/O Layer] 3. bio struct](https://gksruf0326.tistory.com/7)

[[Linux Kernel 5] Block Device Driver Basic Concept](https://pr0gr4m.tistory.com/entry/Linux-Kernel-5-Block-Device-Driver-Basic-Concept)

[Linux Device Driver 정리 (1)](https://tribal1012.tistory.com/154)

[파일시스템 기본 - Sector & Cluster Ⅱ](https://myung-dfc.tistory.com/19)

super block, inode block처럼 파일의 내용이 아닌 파일 시스템을 관리하기 위한 메타 데이터를 읽어올 때는 bio 구조체를 사용하지 않고 `_get_blk()`와 같은 내부 함수를 통해 블록 디바이스와 직접 통신한다. 그리고 이때 가져온 블록 디바이스의 특정 블록 내용을 Buffer Cache 영역에 저장해 둔다.

**정리하자면 Page Cache는 파일의 내용을 저장하고 있는 캐시, Buffer Cache는 파일 시스템의 메타 데이터를 담고 있는 블록을 저장하고 있는 캐시라고 할 수 있다. 그리고 각각이 `free` 에서 표현하고 있는 cached, buffers 영역이다.**

그렇다면 `free` 명령은 왜 이 두 영역을 제외한 영역을 가용한 영역으로 계산해서 다시 보여주는걸까? 서버의 운영 기간이 그리 길지 않을 때는 아마도 1번과 같은 메모리 사용 현황을 볼 수 있을 것이다. 아무 곳에서도 사용하지 않는 가용 영역에 있는 `free` 메모리와 애플리케이션에서 사용하고 있는 영역으로 나뉜다. 하지만 시간이 조금 지나면 **커널은 가용 영역 중 일부를 Cache 영역으로 사용하게 된다.** 그리고 시간이 흐를수록 애플리케이션에서 사용하게 되는 영역이 점점 넓어진다. 어느 순간까지는 가용 영역의 메모리를 가져다가 사용하게 될 것이다. Cache 영역이 충분히 있어야 I/O 성능 향상의 효과를 받을 수 있기 때문이다. **하지만 사용 영역이 점점 더 커져서 일정 수준 이상이 되면 커널은 Cache 영역으로 사용하던 영역을 애플리케이션이 사용할 수 있도록 메모리 관리 시스템에 반환한다.** Cache 영역이 줄고 애플리케이션의 사용 영역이 늘어난다. 이런 과정을 거치다 보면 더 이상 반환할 메모리도 없고 가용할 메모리가 없는 순간이 발생하게 되는데, **시스템은 이때부터 swap이라는 영역을 사용하게 되고 시스템의 성능이 줄어든다.**

이처럼 buffers와 cached 영역은 시스템의 I/O 성능 향상을 위해서 커널이 사용하는 영역이다. **메모리가 부족한 상황이 되면 커널은 해당 영역을 자동으로 반환하기 때문에 `free` 명령에서도 해당 영역을 제외한 영역을 실제 사용 가능한 영역으로 계산하게 된다.**

# 4.3 /proc/meminfo 읽기

리눅스에서는 /proc/meminfo를 통해서 자세한 메모리 현황을 볼 수 있는 방법을 제공하고 있다. 출력 값은 커널 버전에 따라 조금씩 다르다.

```c
MemTotal:         980304 kB
MemFree:          448008 kB
MemAvailable:     646300 kB
Buffers:           18840 kB
Cached:           320984 kB
SwapCached:            0 kB
Active:           322752 kB
Inactive:          82480 kB
Active(anon):      88016 kB
Inactive(anon):        0 kB
Active(file):     234736 kB
Inactive(file):    82480 kB
Unevictable:       40228 kB
Mlocked:           27264 kB
SwapTotal:             0 kB
SwapFree:              0 kB
Zswap:                 0 kB
Zswapped:              0 kB
Dirty:              5232 kB
Writeback:             0 kB
AnonPages:        105704 kB
Mapped:            96516 kB
Shmem:               880 kB
KReclaimable:      17708 kB
Slab:              60068 kB
SReclaimable:      17708 kB
SUnreclaim:        42360 kB
KernelStack:        2556 kB
PageTables:         3296 kB
SecPageTables:         0 kB
NFS_Unstable:          0 kB
Bounce:                0 kB
WritebackTmp:          0 kB
CommitLimit:      490152 kB
Committed_AS:     489496 kB
VmallocTotal:   34359738367 kB
VmallocUsed:        9120 kB
VmallocChunk:          0 kB
Percpu:             8128 kB
HardwareCorrupted:     0 kB
AnonHugePages:         0 kB
ShmemHugePages:        0 kB
```

- SwapCached: swap으로 빠진 메모리 영역 중 다시 메모리로 돌아온 영역을 의미한다. 메모리가 확보되어 다시 메모리로 돌아가지만, swap 영역에서 지우지는 않는다. 시스템에 메모리가 부족하면 커널은 프로세스의 주소 공간 중 swap 영역으로 이동시킬 수 있는 메모리를 선택해서 swap 영역으로 이동시킨다. 이 과정에서 I/O가 일어나기 때문에 성능 저하가 발생한다. 그 후 메모리가 다시 확보되어 swap 영역으로 빠졌던 영역이 다시 메모리로 돌아가게 되더라도 커널은 swap 영역에서 해당 메모리 내용을 삭제하지 않는다. 이후에 또다시 메모리 부족 현상이 일어날 경우를 대비하는 것이다. 해당 메모리 영역이 다시 swap 영역으로 내려와야 하는 일이 생긴다면 swap 영역으로 다시 복사하는 것이 아니라 이전에 참고한 메모리 영역을 삭제하지 않고 그대로 다시 활용한다. 이를 통해서 swap이 발생하더라도 조금이나마 I/O를 줄일 수 있다.
    
    [Swap 영역에 대해 (Swap 메모리)](https://tech-linux.tistory.com/11)
    
- Active(anon): anon은 anonymous의 줄임말이다. Anonymous가 익명이라는 뜻이 있어 오해의 소지가 있지만, 여기서는 **특정 파일의 내용을 저장하고 있는 Page Cache 영역을 제외한 메모리 영역을 의미**한다. 주로 **프로세스들이 사용하는 메모리 영역을 지칭할 때 많이 사용된다.** 그중에서도 비교적 최근에 메모리 영역이 참조되어 swap 영역으로 이동되지 않을 메모리 영역을 의미한다.
- Inactive(anon): Active와 같은 영역을 의미하지만, 비교적 참조된지 오래되어 swap 영역으로 이동될 수 있는 메모리 영역을 의미한다.
- Active(file): anon과는 다르게 file로 되어있는 이 영역은 커널이 I/O 성능 향상을 위해 사용하는 영역을 의미한다. 4.2에서 살펴보면 buffers와 cached 영역이 여기에 속한다. Active라는 이름에서 알 수 있듯이 비교적 최근까지 메모리 영역이 참조외어 Swap 영역으로 이동되지 않을 메모리 영역이다.
- Inactive(file): Active(file)과 마찬가지로 I/O 성능 향상을 위해 커널이 캐시 목적으로 사용하고 있는 영역이다. Inactive라는 단어에서 알 수 있듯이 비교적 참조된 지 오래되어 swap 영역으로 이동될 수 있는 메모리 영역이다.
- Dirty: I/O 성능 향상을 위해 커널이 캐시 목적으로 사용하는 영역 중 쓰기 작업이 이루어져서 실제 블록 디바이스의 블록에 씌어져야 할 영역을 의미한다. **커널은 기본적으로 I/O 쓰기 요청이 발생했을 때 바로 블록 디바이스로 명령을 내리지 않고 일정량이 될 때까지 모았다가 한 번에 쓰는 일종의 지연 쓰기 작업을 한다.** Dirty 메모리는 이 과정에서 사용되는 메모리 영역이다.
    
    [dirty page가 I/O에 미치는 영향](https://byungwoo.oopy.io/5188bc65-5b75-4538-a4c1-4826ea2e551c)

/proc/meminfo 에서 확인할 수 있는 몇가지 정보들을 살펴봤는데, 아직 풀리지 않는 궁금증이 있다. 바로 Active 영역과 Inactive 영역을 구분하는 방법이다.두 영역은 어떤 기준으로 구분될까?

커널이 직접 사용하는 메모리 영역을 제외하고 대부분의 메모리는 프로세스가 사용하거나 4.2에서 언급한 캐시 영역으로 사용한다. 위 결과를 보면, 이 메모리들이 각각 anon과 file을 의미한다.

그림을 보면, anon, file 영역의 메모리는 LRU 기반의 리스트로 관리되고, 이 리스트는 다시 Active, Inactive두 개의 리스트로 나뉜다. anon 영역과 file 영역은 방식은 조금 다르지만 결과적으로는 **자주 사용되는 메모리 영역이 Active 리스트에 남게 된다.** 가장 최근에 참조한 메모리가 Active List에, 그리고 참조 시기가 오래될수록 Inactive 영역으로 이동하고 이후 free 영역으로 이동한다.

그렇다면 Active 리스트와 Inactive 리스트 사이의 이동은 어떻게 결정될까? 기본적으로 프로세스가 메모리 할당을 요청하면 해당 메모리의 페이지가 Active 리스트에 연결된다. 그 후 메모리 할당이 실패하거나 메모리가 부족하게 되면 `kswapd` 혹은 커널 내부에서 `try_to_free_pages()` 함수를 통해서 LRU 리스트에 있는 메모리들을 확인한다.

[/proc/meminfo의 Active와 Inactive, 그리고 kswapd](https://lots-of-knowledge.tistory.com/12)

이 과정에서 Active 리스트에 있던 페이지가 해제되어 다른 프로세스에게 할당되는 작업이 이루어진다.

2장에서 사용한 `malloc()` 함수 테스트용 코드를 살짝 바꿔서 간단하게 테스트해보자.

```bash
buntu@ip-172-31-0-131:~$ cat /proc/meminfo | grep -i anon
Active(anon):      34716 kB
Inactive(anon):    59212 kB
AnonPages:        111572 kB
AnonHugePages:         0 kB
ubuntu@ip-172-31-0-131:~$ cat /proc/meminfo | grep -i anon
Active(anon):      35724 kB
Inactive(anon):    59212 kB
AnonPages:        112620 kB
AnonHugePages:         0 kB
ubuntu@ip-172-31-0-131:~$ cat /proc/meminfo | grep -i anon
Active(anon):      36772 kB
Inactive(anon):    59212 kB
AnonPages:        113668 kB
AnonHugePages:         0 kB
ubuntu@ip-172-31-0-131:~$ cat /proc/meminfo | grep -i anon
Active(anon):      38828 kB
Inactive(anon):    59212 kB
AnonPages:        115720 kB
AnonHugePages:         0 kB
ubuntu@ip-172-31-0-131:~$ cat /proc/meminfo | grep -i anon
Active(anon):      39916 kB
Inactive(anon):    59212 kB
AnonPages:        116848 kB
AnonHugePages:         0 kB
ubuntu@ip-172-31-0-131:~$ cat /proc/meminfo | grep -i anon
Active(anon):      41072 kB
Inactive(anon):    59212 kB
AnonPages:        117892 kB
AnonHugePages:         0 kB
ubuntu@ip-172-31-0-131:~$ cat /proc/meminfo | grep -i anon
Active(anon):      42068 kB
Inactive(anon):    59212 kB
AnonPages:        118940 kB
AnonHugePages:         0 kB
ubuntu@ip-172-31-0-131:~$ cat /proc/meminfo | grep -i anon
Active(anon):      42060 kB
Inactive(anon):    59212 kB
AnonPages:        118956 kB
AnonHugePages:         0 kB
ubuntu@ip-172-31-0-131:~$ cat /proc/meminfo | grep -i anon
Active(anon):      42084 kB
Inactive(anon):    59212 kB
AnonPages:        118940 kB
AnonHugePages:         0 kB
```

위 결과를 보면 Active(anon) 메모리 영역이 계속해서 증가하는 것을 볼 수 있다. 그리고 10번의 할당이 끝난 후에 sleep 함수를 통해 아무것도 하지 않아도 메모리 영역은 계속 Active 영역이 남아있다. 단순히 시간이 지난다고 해서 Active 메모리가 Inactive로 이동하지 않는다는 것을 확인할 수 있다.

그럼 Active에 있는 페이지는 언제 Inactive로 이동할까? 위에서도 언급했지만 **메모리 부족 현상이 발생해서 해제해야 할 메모리를 찾아야 하는 순간이 와야 커널은 비로소 LRU 리스트를 살펴보게 된다.** 그럼 kswapd를 강제로 실행시켜보자. 커널 파라미터 중에는 `vm.min_free_kbytes` 라는 파라미터가 있다. 해당 파라미터는 이름에서 알 수 있는 것처럼 **시스템에서 유지해야 하는 최소한의 free 메모리 양이다.** 이 값을 조금 높게 설정해보자.

```bash
root@ip-172-31-0-131:/home/ubuntu# sysctl -w vm.min_free_kbytes=6553500
vm.min_free_kbytes = 6553500
```

그리고 나서 top을 통해 살펴보면 kswapd가 실행되는 것을 확인할 수 있다.

```bash
ubuntu@ip-172-31-0-131:~$ top -b -n 1 | grep kswapd
     46 root      20   0       0      0      0 S   0.0   0.0   0:00.00 kswapd0
```

kswapd 데몬이 열심히 일을 하면서 Active 영역에 있는 페이지 중 오래된 페이지를 우선적으로 Inactive로 옮긴 후 메모리를 해제하는 작업을 진행하고 있다.

```bash
ubuntu@ip-172-31-0-131:~$ cat /proc/meminfo | grep -i anon
Active(anon):      87812 kB
Inactive(anon):        0 kB
AnonPages:        105516 kB
AnonHugePages:         0 kB
```

거의 12MB에 달하던 active(anon) 영역이 348kb 수준으로 내려갔고 (나는 안 내려가네….^^) free 영역이 `vm.min_free_kbytes`에 설정한 값 이상으로 올라간 것을 볼 수 있다. `vm.min_free_kbytes` 값을 원래대로 설정하면 kswapd가 더는 실행되지 않는다. 그리고 해당하는 메모리들이 대부분 Swap 영역으로 빠진다. 그래서 위의 kswapd 데몬 상태가 D 상태로 보인다.

# 4.4 slab 메모리 영역

지금까지 우리는 buffers, cached라고 부르는 캐싱 영역과 anon이라고 부르는 프로세스의 메모리 영역을 살펴봤다. 커널 역시 프로세스의 일종이기 때문에 메모리를 필요로하며, 조금 특별한 방법으로 메모리를 할당 받아서 사용한다. 그리고 우리가 앞 절에서 본 /proc/meminfo의 내용 중 아직 언급하지 않은, Slab 으로 표시되는 영역이 바로 커널이 사용하는 영역이다.

```bash
Slab:             117244 kB
SReclaimable:      68440 kB
SUnreclaim:        48804 kB
```

- Slab: 메모리 영역 중 커널이 직접 사용하는 영역을 Slab 영역이라고 한다. 이 영역에는 dentry cache, inode cache 등 커널이 사용하는 메모리가 포함된다.
- SReclaimable: Slab 영역 중 재사용될 수 있는 영역이다. 캐시 용도로 사용하는 메모리들이 주로 여기에 포함된다. 메모리 부족 현상이 일어나면 해제되어 프로세스에 할당될 수 있는 영역이다.
- SUnreclaim: Slab 영역 중 재시용될 수 없는 영역이다. 커널이 현재 사용중인 영역이며, 해제해서 다른 용도로 사용할 수 없다.

리눅스에서는 `slabtop` 명령을 통해 현재 시스템에서 사용 중인 Slab의 정보를 살펴볼 수 있다.

```bash
root@ip-172-31-0-131:/home/ubuntu# slabtop -o
 Active / Total Objects (% used)    : 315488 / 385253 (81.9%)
 Active / Total Slabs (% used)      : 10449 / 10449 (100.0%)
 Active / Total Caches (% used)     : 320 / 402 (79.6%)
 Active / Total Size (% used)       : 83092.90K / 102662.52K (80.9%)
 Minimum / Average / Maximum Object : 0.01K / 0.27K / 10.38K

  OBJS ACTIVE  USE OBJ SIZE  SLABS OBJ/SLAB CACHE SIZE NAME                   
 49560  35865  72%    0.09K   1180       42      4720K trace_event_file       
 45357  45357 100%    0.10K   1163       39      4652K buffer_head            
 39270  32980  83%    0.19K   1870       21      7480K dentry                 
 35700  13478  37%    0.04K    350      102      1400K extent_status          
 35280  22544  63%    1.14K   1260       28     40320K ext4_inode_cache       
 31980  31980 100%    0.13K   1066       30      4264K kernfs_node_cache      
 19328   8028  41%    0.06K    302       64      1208K dmaengine-unmap-2      
 12656  12656 100%    0.57K    452       28      7232K radix_tree_node        
  7728   7728 100%    0.07K    138       56       552K vmap_area              
  6550   6534  99%    0.62K    262       25      4192K inode_cache        
```

모든 프로세스는 작업을 하기 위해 메모리가 필요하고 이는 커널도 예외가 아니다. I/O 작업을 조금이라도 더 빠르게 하기 위해 inode cache, dentry cache 등을 사용하거나 네트워크 소켓을 위한 메모리 영역을 확보하거나 하는 작업들은 커널(좀 더 엄밀히 말하자면 디바이스 드라이버)이 하게 되는데, 이 과정에서 메모리가 필요하다. 하지만 메모리를 할당해주는 버디 시스템은 4KB의 페이지 단위로 메모리를 할당한다. 사실 커널 입장에서는 이렇게 큰 영역을 할당 받을 필요가 없다. 또한, 영역이 크다면 실제 사용하는 영역과 할당 받은 영역의 차이가 커지면서 메모리 단편화 현상도 발생할 수 있다. 그렇기 때문에 커널이 사용하려는 메모리 영역은 좀 더 작고 효율적으로 사용할 수 있어야 한다. **이를 충족시키기 위해서 커널은 Slab 할당자를 통해서 원하는 메모리 영역을 확보한다.**

메모리의 기본 단위인 4KB의 영역을 slab 할당자가 어떻게 나눠서 사용하는지를 간단히 정리한 그림이다. 그림을 보면 Slab 할당자는 각각의 목적에 맞는 캐시별로 영역을 할당 받아 사용한다. dentry cache, inode cache 등이 목적별로 나뉘어 있다. 이렇게 버디 시스템을 통해서 페이지 크기인 기본 4KB의 영역을 할당 받은 후에 각각의 캐시 크기에 맞게 영역을 나눠서 사용한다. 그래서 경우에 따라서는 페이지 크기의 배수로 딱 떨어지지 않는 경우도 존재할 수 있다.

간단한 테스트를 통해 Slab 영역이 늘어나는 것을 확인해보자. Slab 영역 중 가장 많이 사용되는 캐시가 dentry와 inode_cache이다. 각각은 **디렉터리의 계층 관계를 저장해두는 캐시**, **파일의 inode에 대한 정보를 저장해두는 캐시**이다.

```bash
root@ip-172-31-0-131:/home/ubuntu# slabtop -o | grep dentry
 39270  32928  83%    0.19K   1870       21      7480K dentry                 
root@ip-172-31-0-131:/home/ubuntu# cd /boot
root@ip-172-31-0-131:/boot# ls -l
total 147357
-rw------- 1 root root  9205614 Sep 23 21:02 System.map-6.14.0-1015-aws
-rw------- 1 root root  9204793 Nov 24 18:01 System.map-6.14.0-1018-aws
-rw-r--r-- 1 root root   270863 Sep 23 21:02 config-6.14.0-1015-aws
-rw-r--r-- 1 root root   270932 Nov 24 18:01 config-6.14.0-1018-aws
drwx------ 3 root root      512 Jan  1  1970 efi
drwxr-xr-x 6 root root     4096 Jan  4 08:19 grub
lrwxrwxrwx 1 root root       26 Jan  4 08:19 initrd.img -> initrd.img-6.14.0-1018-aws
-rw-r--r-- 1 root root 43744280 Oct 22 10:15 initrd.img-6.14.0-1015-aws
-rw-r--r-- 1 root root 43740203 Jan  4 08:20 initrd.img-6.14.0-1018-aws
lrwxrwxrwx 1 root root       26 Oct 22 10:14 initrd.img.old -> initrd.img-6.14.0-1015-aws
drwx------ 2 root root    16384 Oct 22 10:03 lost+found
-rw-r--r-- 1 root root 13274112 Jan  4 08:20 microcode.cpio
lrwxrwxrwx 1 root root       23 Jan  4 08:19 vmlinuz -> vmlinuz-6.14.0-1018-aws
-rw------- 1 root root 15567240 Sep 23 21:04 vmlinuz-6.14.0-1015-aws
-rw------- 1 root root 15575432 Nov 24 18:03 vmlinuz-6.14.0-1018-aws
lrwxrwxrwx 1 root root       23 Oct 22 10:14 vmlinuz.old -> vmlinuz-6.14.0-1015-aws
root@ip-172-31-0-131:/boot# slabtop -o | grep dentry
 39270  32928  83%    0.19K   1870       21      7480K dentry 
```

위 예제처럼 간단하게 `cd`로 이동해서 `ls` 명령으로 디렉터리를 살펴보는 것만으로도 dentry 값은 증가한다. (실제로 해보니 증가하지는 않았다.) 만약 **파일에 자주 접근하고 디렉터리의 생성/삭제가 빈번한 시스템이 있다면 Slab 메모리가 높아질 수 있으며, 그 중에서도 dentry, inode_cache가 높아질 수 있다.**

또한 Slab 할당자는 `free` 명령에서는 used로 계산된다. 커널이 사용하는 캐시 영역이기 때문에 buffers/cached 영역에 포함될 것이라고 생각할 수 있지만 used 영역으로 계산된다. 그래서 간혹 프로세스들이 사용하는 메모리 영역을 모두 더하고도 used와 맞지 않을 경우 Slab 메모리에서 누수가 발생하는 것일 수도 있다.