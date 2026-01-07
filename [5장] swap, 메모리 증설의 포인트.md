# 5.1 swap 영역

swap 영역은 물리 메모리가 부족할 경우를 대비해서 만들어 놓은 영역이다. 메모리는 프로세스가 연산을 하기 위해 만들어 놓은 일종의 저장 공간과 같은 것인데, 이 공간이 모자라면 프로세스는 더 이상 연산을 위한 공간을 확보할 수 없기 때문에 전체 시스템이 응답 불가 상태에 빠질 수 있다. 이런 응답 불가 상태에 빠지지 않고 시스템이 안정적으로 운영될 수 있도록 비상용으로 확보해 놓은 메모리 공간이 swap 영역이다. 하지만 swap 영역은 물리 메모리가 아니라 디스크의 일부분을 메모리처럼 사용하기 위해 만들어 놓은 공간이기 때문에, 메모리가 부족할 때 사용한다고는 하지만 메모리에 비해 접근과 처리 속도가 현저하게 떨어진다. 그래서 swap 영역을 사용하게 되면 시스템의 성능 저하가 일어난다.

리눅스에서 사용 중인 swap 영역 정보는 `free` 명령을 통해서 확인할 수 있다.

```c
ubuntu@ip-172-31-0-176:~$ free -k
               total        used        free      shared  buff/cache   available
Mem:          980300      704200       90408        1008      361972      276100
Swap:              0           0           0
ubuntu@ip-172-31-0-176:~$ 
```

- total: 전체 swap 영역의 크기를 의미한다. `-k` 옵션은 KB 단위이기 때문에 약 10GB 정도의 영역을 확보해둔 것을 알 수 있다. (실제 내 서버는 swap 영역이 없다.)
- used: 현재 사용중인 swap 영역의 크기를 의미한다. swap 영역을 사용했다는 것 자체가 시스템에 메모리와 관련해 문제가 있을 수 있다는 의미이다. 아주 적인 양이라도 swap 영역을 쓰기 시작했다면 반드시 살펴봐야 한다.
- free: 현재 남아있는 swap 영역의 크기이다.

**swap 영역을 사용한다는 것 자체가 시스템의 메모리가 부족할 수 있다는 의미이기 때문에 어떤 프로세스가 사용하고 있는지 확인해볼 필요가 있다.** 서비스 용도가 아닌 관리 용도의 프로세스에 메모리 누수가 있어서 메모리를 계속해서 점유하려 하고, 그 과정에서 swap을 사용하고 있을 수도 있기 때문이다. 이런 경우라면 관리 용도의 프로세스를 죽여서 메모리 부족 현상으로 인한 성능 저하를 해결할 수 있다. swap의 사용 여부를 판단하는 것도 중요하지만 누가 swap을 사용하느냐도 메우 중요한 판단 기준이 된다.

모든 프로세스는 /proc/{pid}의 디렉터리에 자신과 관련된 정보들을 저장한다. 프로세스가 사용하는 메모리에 대한 정보도 이곳에 저장되는데 그중에서도 /proc/{pid}/smaps 파일이 바로 메모리 정보를 저장하고 있다.

```c
ubuntu@ip-172-31-0-176:/proc/1397$ cat smaps | more
5c3ddd614000-5c3ddd615000 r--p 00000000 00:2f 266762                     /usr/local/bin/python3.11
Size:                  4 kB
KernelPageSize:        4 kB
MMUPageSize:           4 kB
Rss:                   0 kB
Pss:                   0 kB
Pss_Dirty:             0 kB
Shared_Clean:          0 kB
Shared_Dirty:          0 kB
Private_Clean:         0 kB
Private_Dirty:         0 kB
Referenced:            0 kB
Anonymous:             0 kB
KSM:                   0 kB
LazyFree:              0 kB
AnonHugePages:         0 kB
ShmemPmdMapped:        0 kB
FilePmdMapped:         0 kB
Shared_Hugetlb:        0 kB
Private_Hugetlb:       0 kB
Swap:                  0 kB
SwapPss:               0 kB
Locked:                0 kB
THPeligible:           0
VmFlags: rd mr mw me sd
```

프로세스가 사용하고 있는 메모리 영역 중 해당 번지에 속한 메모리 영역이 swap 영역에 있는지 아닌지를 확인할 수 있다. 해당 프로세스의 논리적 메모리 5c3ddd614000-5c3ddd615000 사이에 있는 메모리는 크기가 4KB이며 swap 영역은 없다.

이렇게 /proc/{pid}/smaps 파일의 정보를 통해서 각 프로세스의 메모리 영역별로 사용하는 swap 영역을 확인할 수 있다. 하지만 프로세스의 메모리 영역별로 살펴봐야 하기 때문에 불편하다. 그래서 특정 프로세스가 사용하는 전체 swap 영역에 대한 정보가 필요할 경우에는 /proc/{pid}/status 파일을 통해서도 확인할 수 있다.

```c
ubuntu@ip-172-31-0-176:/proc/1397$ cat status
Name:   python
Umask:  0022
State:  S (sleeping)
Tgid:   1397
Ngid:   0
Pid:    1397
PPid:   1320
TracerPid:      0
Uid:    1000    1000    1000    1000
Gid:    1000    1000    1000    1000
FDSize: 256
Groups: 1000 
NStgid: 1397    8
NSpid:  1397    8
NSpgid: 1320    1
NSsid:  1320    1
Kthread:        0
VmPeak:   470684 kB
VmSize:   470684 kB
VmLck:         0 kB
VmPin:         0 kB
VmHWM:    141868 kB
VmRSS:    114896 kB
RssAnon:          111716 kB
RssFile:            3180 kB
RssShmem:              0 kB
VmData:   144160 kB
VmStk:       132 kB
VmExe:         4 kB
VmLib:     31508 kB
VmPTE:       424 kB
**VmSwap:        0 kB**
HugetlbPages:          0 kB
CoreDumping:    0
THP_enabled:    1
untag_mask:     0xffffffffffffffff
Threads:        5
SigQ:   0/3739
SigPnd: 0000000000000000
ShdPnd: 0000000000000000
SigBlk: 0000000000000000
SigIgn: 0000000001001000
SigCgt: 0000000100004002
CapInh: 0000000000000000
CapPrm: 0000000000000000
CapEff: 0000000000000000
CapBnd: 00000000a80425fb
CapAmb: 0000000000000000
NoNewPrivs:     0
Seccomp:        2
Seccomp_filters:        1
Speculation_Store_Bypass:       vulnerable
SpeculationIndirectBranch:      always enabled
Cpus_allowed:   0001
Cpus_allowed_list:      0
Mems_allowed:   00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000001
Mems_allowed_list:      0
voluntary_ctxt_switches:        30971412
nonvoluntary_ctxt_switches:     375374
x86_Thread_features:
x86_Thread_features_locked:
```

많은 출력 결과 중 VmSwap 항목이 해당 프로세스가 사용하는 swap 영역에 대한 정보를 의미한다.

이 외에 전체 프로세스별로 사용 중인 swap 영역의 크기를 확인하고 싶은 경우가 있는데, 이런 경우에 유용한 `smem`이라는 유틸리티가 있다. 이 유틸리티는 /proc/{pid}의 내용을 바탕으로 각 프로세스들의 메모리 사용 현황을 보여준다.

```c
ubuntu@ip-172-31-0-176:~$ smem -t
  PID User     Command                         Swap      USS      PSS      RSS 
1973892 ubuntu   -bash                              0     2336     2439     4540 
 1396 ubuntu   /usr/local/bin/python -c fr        0     7584     7902     8864 
1975779 ubuntu   /usr/bin/python3 /usr/bin/s        0    14008    14392    17076 
 1320 ubuntu   /usr/local/bin/python /usr/        0    23820    24171    25208 
 1398 ubuntu   /usr/local/bin/python -c fr        0   111644   112683   114412 
 1397 ubuntu   /usr/local/bin/python -c fr        0   111916   112955   114684 
-------------------------------------------------------------------------------
    6 1                                           0   271308   274542   284784 
```

제일 마지막 줄이 전체 합계이다.
