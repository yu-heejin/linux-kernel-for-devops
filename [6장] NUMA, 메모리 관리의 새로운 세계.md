# 6.1 NUMA 아키텍처

NUMA는 Non-Uniform Memory Access의 약자로, 번역하면 **불균형 메모리 접근**이라는 뜻이며 **멀티 프로세스 환경에서 적용되는 메모리 접근 방식이다.**

그림은 UMA(Uniform Memory Access), 즉 NUMA와 반대되는 개념으로 초창기 아키텍처라고 볼 수 있다. **이 방식에서는 모든 프로세서가 공용 BUS를 이용해서 메모리에 접근한다.** 이 방식의 문제점은 BUS를 동시에 사용할 수 없는 것으로, 0번 소켓에 있는 CPU가 메모리에 접근하는 동안 1번 소켓에 있는 CPU는 메모리에 접근할 수가 없다.

그림은 우리가 이번 장에서 이야기할 NUMA의 개념도이다. UMA와 다른 점은 **로컬 메모리로의 접근이 동시에 이뤄질 수 있다는 것이다.** 0번 CPU가 자신의 로컬 메모리에 접근하는 동안 1번 CPU도 자신의 메모리에 접근할 수 있어서 성능이 향상된다. 하지만 **로컬 메모리의 양이 모자라면 다른 CPU에 붙어있는 메모리에 접근이 필요하게 되고, 이때 메모리 접근에 시간이 소요되어 예상치 못한 성능 저하를 경험하게 된다.** 그래서 **로컬 메모리에서 얼마나 많이 메모리 접근이 일어나느냐가 성능 향상의 가장 중요한 포인트이다.**

<aside>
💡

각각의 cpu마다 별도의 메모리가 있는데 이와 같이 메모리에 접근하는 방식을 로컬 액세스(Local Access)라고 한다. 그리고 이렇게 CPU와 메모리를 합쳐서 노드라고 부른다. NUMA에서는 자신의 메모리가 아닌 다른 노드에 있는 메모리에도 접근할 수 있으며, 이것을 리모트 액세스라고 부른다.

</aside>

# 6.2 리눅스에서의 NUMA 확인

리눅스에서는 NUMA를 활용하기 위한 코드를 구현해놓았고 명령어를 통해서 현재 시스템의 NUMA 상태를 확인할 수 있다.

먼저 `numactl` 명령어를 살펴보자. `numactl`은 NUMA와 관련된 정책을 확인하거나 설정할 때 사용한다. `--show` 명령으로 NUMA 정책을 확인해보자.

```c
ubuntu@ip-172-31-0-13:~$ numactl --show
policy: default  // 1
preferred node: current
physcpubind: 0 
cpubind: 0 
nodebind: 0 
membind: 0 
preferred: 
```

1. 기본 정책이 default인 것을 알 수 있다. default는 현재 사용중인 프로세스가 포함된 노드에서 메모리를 먼저 가져다가 사용하는 방식이다.

NUMA와 관련된 메모리 할당 정책은 총 4가지이다.

[[운영체제] 프로세스와 프로세서의 차이](https://lala9663.tistory.com/103)

1. default 정책: 별도의 설정을 하지 않는 한 모든 프로세스에 적용되며, 현재 프로세스가 실행되고 있는 프로세서(CPU)가 포함된 노드에서 먼저 메모리를 할당 받아 사용한다.
2. bind 정책: 특정 프로세스를 특정 노드에 바인딩시키는 방식을 취한다. 예를 들어 0번 노드에 할당하면 0번 노드에서만 메모리를 할당 받는다. 이 경우 메모리의 지역성이 좋아지기 때문에 메모리 접근 속도가 빨라서 성능이 좋아질 수 있지만, bind에 설정한 노드의 메모리가 부족하면 성능이 급격히 나빠질 수도 있다.
3. preferred 정책: bind와 비슷하지만 선호하는 노드를 설정한다. bind가 반드시 설정한 노드에서 메모리를 할당받는 반면에 preferred는 가능한 한 설정한 노드로부터 메모리를 할당 받는다.
4. interleaved 정책: 다수의 노드에서 거의 동일한 비율로 메모리를 할당 받는다. 라운드 로빈 정책에 따라 다수의 노드로부터 한 번씩 돌아가면서 메모리를 할당받는다.

[4. NUMA Architecture.](https://brunch.co.kr/@dreaminz/4)

다음은 `-H` 옵션이다.

```c
ubuntu@ip-172-31-0-13:~$ numactl -H
available: 1 nodes (0)    // 1
node 0 cpus: 0     // 2
node 0 size: 957 MB
node 0 free: 133 MB
node distances:    // 3
node   0 
  0:  10 
```

1. NUMA 노드가 1개로 구성되어 있음을 볼 수 있다.
2. 각각의 노드 0에 해당하는 CPU의 번호와 각 노드에 할당된 메모리의 크기이다. CPU 번호는 시스템마다 조금씩 다를 수 있다.
3. 각 노드의 메모리에 접근하는데 걸리는 시간을 의미한다. 각각의 로컬 메모리에 접근할 때 소요되는 시간을 10이라고 한다면, 즉 0번 노드에서 1번 노드에 있는 메모리에 접근하거나 1번 노드에서 0번 노드에 있는 메모리에 접근할 때 소요되는 시간은 20임을 보여준다. 이는 절대적인 시간이 아니라 상대적인 값으로, 리모트 메모리에 접근하는 시간이 로컬 메모리에 접근하는데 필요한 시간의 2배라는 뜻이다.

다음은 NUMA 환경에서 현재 시스템에 할당된 메모리의 상태를 확인할 때 사용하는 명령어인 `numastat`이다.

```c
ubuntu@ip-172-31-0-13:~$ numastat -cm

Per-node system memory usage (in MBs):
Token Unaccepted not in hash table.
                 Node 0 Total
                 ------ -----
MemTotal            957   957
MemFree             133   133
MemUsed             824   824
SwapCached            0     0
Active              318   318
Inactive            357   357
Active(anon)         72    72
Inactive(anon)        2     2
Active(file)        246   246
Inactive(file)      355   355
Unevictable          39    39
Mlocked              27    27
Dirty                 0     0
Writeback             0     0
FilePages           623   623
Mapped               90    90
AnonPages            91    91
Shmem                 1     1
KernelStack           2     2
PageTables            3     3
SecPageTables         0     0
NFS_Unstable          0     0
Bounce                0     0
WritebackTmp          0     0
Slab                 70    70
SReclaimable         26    26
SUnreclaim           44    44
AnonHugePages         0     0
ShmemHugePages        0     0
ShmemPmdMapped        0     0
FileHugePages         0     0
FilePmdMapped         0     0
HugePages_Total       0     0
HugePages_Free        0     0
HugePages_Surp        0     0
KReclaimable         26    26
```

`numastat` 명령이 중요한 이유는 **NUMA 아키텍처에서 메모리 불균형 상태를 확인할 수 있기 때문이다.** 어느 한 쪽 노드의 메모리 사용률이 높으면 메모리 할당 정책에 따라 swap을 사용하는 경우도 있기 때문이다. 분명이 전체 메모리에는 free 영역이 많이 있는데도 불구하고 메모리 할당 정책에 따라 한쪽 노드에서 메모리 할당이 과하게 일어나면 swap을 사용하게 되며, 이런 상태를 `numastat`을 통해서 확인할 수 있다.

이번에는 프로세스가 어떤 메모리 할당 정책으로 실행되었는지 확인하는 방법을 살펴보자. /proc/{pid}/numa_maps에는 현재 동작 중인 프로세스의 메모리 할당 정책과 관련된 정보가 기록된다.

```c
ubuntu@ip-172-31-0-13:/proc$ sudo cat /proc/568/numa_maps 
5a45b5386000 default file=/usr/lib/systemd/systemd-logind mapped=12 active=0 N0=12 kernelpagesize_kB=4
5a45b5392000 default file=/usr/lib/systemd/systemd-logind mapped=38 active=0 N0=38 kernelpagesize_kB=4
5a45b53b8000 default file=/usr/lib/systemd/systemd-logind mapped=14 active=0 N0=14 kernelpagesize_kB=4
5a45b53c7000 default file=/usr/lib/systemd/systemd-logind anon=5 dirty=5 active=0 N0=5 kernelpagesize_kB=4
5a45b53cc000 default file=/usr/lib/systemd/systemd-logind anon=1 dirty=1 active=0 N0=1 kernelpagesize_kB=4
5a45cbf15000 default heap anon=46 dirty=46 active=0 N0=46 kernelpagesize_kB=4
...
```

위 프로세스는 default 정책으로 실행된 것이다.