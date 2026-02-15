TCP는 그 특성상 자신이 보낸 데이터에 대해서 상대방이 받았다는 의미의 응답 패킷을 다시 받아야 통신이 정상적으로 이루어졌다고 생각한다. 그래서 만약 자신이 보낸 데이터에 대한 응답 패킷을 받지 못하면 패킷이 유실되었다고 판단하고 보냈던 패킷을 다시 한번 보낸다. 이 과정을 TCP 재전송이라고 한다. TCP 재저송은 보냈던 패킷을 다시 한번 보내기 때문에 네트워크 성능에 저하를 가져올 수밖에 없지만, TCP 통신의 특성상 반드시 필요한 과정이다.

# 9.1 TCP 재전송과 RTO

TCP는 흔히 신뢰성 있는 연결이라고 한다. UDP와는 다르게 데이터를 주고받는 두 종단간에 데이터를 주고 받음이 확실해야 한다. 보낸 쪽에서는 내가 보낸 데이터를 상대방이 받았다는 응답을 보내야만 나머지를 보낼 수 있다. 이를 통해 TCP를 통해 주고 받은 데이터는 정확한 데이터라고 확신하게 된다.

그림을 보면 데이터를 보낸 쪽에서는 받는 쪽의 ACK을 기다린다. ACK을 받아야 내가 보낸 데이터가 정상적으로 도착했음을 확인할 수 있다. ACK을 주고 받는 과정은 selected-ack, delayed-ack 등 여러가지 방법으로 진행될 수 있지만 여기서는 패킷 하나마다 하나를 전달해주는 과정을 가정한다.

패킷을 보낸 후 ACK을 받지 못하면 어떻게 될까? 패킷이 중간에 손실되았다고 판단하고 재전송한다. 이것이 TCP 재전송이며, 애플리케이션 입장에서는 요청에 대한 응답을 받는 시간이 늘어난다. 여기서 ACK을 얼마나 기다려야 하는지에 대한 값을 RTO(Retransmission Timeout)라고 부른다. RTO 안에 ACK을 받지 못하면 보내는 쪽에서 재전송을 진행한다.

RTO에는 일반적인 RTO와 InitRTO 두 가지가 있다. 일반적인 RTO는 RTT(RoundTripTime, 두 종단 간 패킷 전송에 필요한 시간)를 기준으로 설정된다. 예를 들어 두 종단 간 패킷 전송에 필요한 시간이 1초라면, 최소한 1초는 기다려야 내가 보낸 패킷이 손실되었는지 아닌지를 판단할 수 있다. 패킷을 보내는데 1초가 걸리는데 1초보다 더 짧은 시간을 기다리면 당연히 ACK을 받지 못하게 되고 패킷이 유실되었다고 판단해 재전송이 일어나기 때문이다.

InitRTO는 두 종단 간 최초의 연결을 시작할 때, 즉 TCP Handshake가 일어나는 첫 번째 SYN 패킷에 대한 RTO를 의미한다. 맨 처음 연결을 맺을 때는 두 종단 간 RTT와 같은 패킷 전송의 소요 시간을 전혀 알 수 없기 때문에 임의로 설정한 값으로 RTO를 계산한다. 이때의 RTO를 InitRTO라고 하며, 리눅스에서는 소스 코드에 1초로 구현해 놓았다. 즉 SYN 패킷에 대한 RTO는 특별히 1초로 설정된다고 볼 수 있다.

리눅스에서는 `ss`명령을 이용해 현재 설정되어 있는 세션의 RTO값을 확인할 수 있다.

```yaml
root@ip-172-31-0-62:/home/ubuntu# ss -i
tcp        ESTAB       0           0                                                          172.31.0.62:ssh                  13.209.1.60:16376     
         cubic wscale:7,7 **rto:202  // 1** rtt:1.045/0.124 ato:76 mss:1448 pmtu:9001 rcvmss:688 advmss:8949 cwnd:10 bytes_sent:42787 bytes_acked:42787 bytes_received:13112 segs_out:379 segs_in:625 data_segs_out:371 data_segs_in:259 send 111Mbps lastsnd:35 lastrcv:35 lastack:33 pacing_rate 222Mbps delivery_rate 75.3Mbps delivered:372 app_limited busy:527ms rcv_space:17408 rcv_ssthresh:17408 minrtt:0.782 snd_wnd:56704
```

1에서 보이는 rto 값이 위에서 언급한 RTO다. 이 세션의 RTO 값은 202ms이다. 202ms 동안 ACK을 받지 못하면 패킷을 재전송하게 된다.

RTO 값은 초기값을 기준으로 2배씩 증가한다. 그래서 처음 값은 202ms, 그 이후는 404, 808, … 으로 점점 더 커진다.

# 9.2 재전송을 결정하는 커널 파라미터

재전송과 관련된 커널 파라미터는 총 5개의 값이 있다.

```yaml
root@ip-172-31-0-62:/home/ubuntu# sysctl -a | grep -i retries
net.ipv4.tcp_orphan_retries = 0
net.ipv4.tcp_retries1 = 3
net.ipv4.tcp_retries2 = 15
net.ipv4.tcp_syn_retries = 6
net.ipv4.tcp_synack_retries = 5
net.ipv6.idgen_retries = 3
```

## net.ipv4.tcp_syn_retries

TCP 재전송은 이미 연결되어 있는 세션에서도 일어나지만 연결을 시도하는 과정에서도 일어난다. 그리고 해당 파라미터는 바로 SYN에 대한 재시도 횟수를 결정한다. 기본값은 5이다. TCP 스펙에서도 이 값은 최소한 5로 설정하도록 권고하고 있다. 즉, 5번의 재전송 이후에도 연결이 되지 않으면 연결하지 않는다.

## net.ipv4.tcp_synack_retries

상대편이 보낸 SYN에 대한 응답으로 보내는 SYN+ACK의 재전송 횟수를 정의한다. 기본값은 5이고, 상대방의 SYN에 최대 5번의 SYN+ACK 재전송을 한다는 의미이다. 이 값이 중요한 이유는 SYN을 받은 소켓 처리와 관련이 있기 때문이다. SYN을 받은 후 그에 대해 SYN+ACK로 응답한 소켓의 상태는 SYN_RECV가 된다. 하지만 SYN+ACK에 대해 정상적인 응답을 받지 못하면 재전송한다. 이때 정상적인 상황이라면 상대방 역시 본이느이 SYN에 대한 SYN+ACK을 받지 못했기 때문에 SYN 자체가 재전송되지만, 비정상적인 경우에는 SYN 재전송이 일어나지 않는다. 이때 `net.ipv4.tcp_synack_retires` 값이 없다면 SYN_RECV 상태의 소켓이 계속해서 유지되고 결국 서버의 리소스 고갈이 일어난다. 따라서 이 값 역시 적당한 수준으로 줄여도 좋다.

## net.ipv4.tcp_orphan_retries

orphan socket이라 불리는 상태의 소켓들에 대한 재전송 횟수를 결정한다. 그럼 orphan socket은 무엇일까? 그 전에 TCP가 연결을 끊는 과정을 다시 한번 살펴보자.

그림을 보면TCP 연결을 끊을 때 FIN을 보내고 해당 소켓은 FIN_WAIT1 상태로 변경된다. 이때부터 소켓은 특정 프로세스에 바인딩되지 않고 커널에 귀속된다. 코드에서 `netstat` 명령으로 소켓의 연결 상태들을 살펴보면 ESTABLISHED 상태의 소켓들은 우측에 속한 PID와 프로세스의 이름이 보이지만 FIN_WAIT1, TIME_WAIT 등의 소켓들은 PID와 프로세스 이름이 보이지 않는 것을 확인할 수 있다.

```yaml
root@ip-172-31-0-62:/home/ubuntu# netstat -napo | grep 80
tcp6       0      0 :::80                   :::*                    LISTEN      2990/apache2         off (0.00/0/0)
tcp6       0      0 172.31.0.62:80          172.31.0.19:42074       ESTABLISHED 2995/apache2         keepalive (111.61/0/0)
tcp6       0      0 172.31.0.62:80          172.31.0.19:58808       TIME_WAIT   -                    timewait (36.01/0/0)
```

이렇게 특정 프로세스에 할당되지 않고 커널에 귀속되어 정리되기를 기다리는 소켓 중에서도 FIN_WAIT1 상태의 소켓을 orphan socket이라고 하며, 이 값으로 재전송하는 횟수를 정의한다. 왜 FIN_WAIT2와 TIME_WAIT는 아니고 F

그림에서도 볼 수 있지만 FIN을 보내고 난 후에 FIN_WAIT1이 되고 상대방으로부터 응답을 받으면 FIN_WAIT2에서 TIME_WAIT 상태가 된다. 이 과정을 잘 보면 연결을 끊을 때 자신이 보내는 마지막 패킷은 FIN_WAIT1 상태가 되기 위한 FIN이 된다. 그 이후로는 보내는 패킷은 없고 상대방으로부터 받는 패킷만 있다. 재전송은 내가 보내는 패킷에 대해 재전송하는 것이기 때문에 마지막으로 보내는 패킷에 해당하는 FIN_WAIT1 상태의 소켓만 해당되는 것이다.

사실 FIN을 보낸 후 아주 짧은 시간에 FIN_WAIT1 상태에서 FIN_WAIT2 상태로 빠지고 TIME_WAIT 상태로 이어진다. FIN 전송과 그에 대한 ACK을 받는 과정이 굉장히 빠르게 이루어지기 때문이다. 하지만 테스트에서는 FIN에 대한 ACK을 받을 수 없기 때문에 해당 소켓은 계속해서 FIN_WAIT1 상태로 남아있게 된다. 이 소켓은 `net.ipv4.tcp_orphan_retires`에 정의한 횟수만큼 재전송을 시도하고 그 후에도 응답을 받지 못하면 비정상적이라고 판단하고 커널이 해당 소켓을 강제로 회수한다.

결과적으로 `net.ipv4.tcp_orphan_retries` 에 설정한 값보다 1번 더 보낸다고 생각할 수 있다.

FIN_WAIT1 상태에서 지정된 재전송 횟수까지 모두 보내고 나면 해당 소켓은 어떻게 될까? FIN_WAIT2, TIME_WAIT 상태로 변경되지 않고 이미 죽은 소켓으로 판단해 소켓을 아예 회수해버린다. 그렇기 때문에 `net.ipv4.tcp_orphan_retires` 값이 너무 작으면 FIN 패킷이 유실된 상태의 FIN_WAIT1 소켓이 너무 빨리 정리될 수 있으며, 상대편에 닫혀야 되는 소켓이 닫히지 않는 결과를 초래할 수도 있다. 그래서 최소한 TIME_WAIT이 유지되는 시간인 60초 정도가 될 수 있도록 7 정도의 값을 설정하는 것이 좋다. 그래야 최소한 TIME_WAIT가 남아있는 만큼의 효과를 유지할 수 있다.

## net.ipv4.tcp_retries1, net.ipv4.tcp_retries2

이 두 값은 서로 연관이 있다. TCP는 기본적으로 재전송을 하기 위한 임계치 값으로 두 개의 값을 가지고 있다. 두 값 모두 최종적으로는 재전송 횟수를 정의하지만, **첫번째 값은 IP 레이어에 네트워크가 잘못 되었는지 확인하도록 사인을 보내는 기준이 되며, 두 번째 값은 더 이상 통신을 할 수 없다고 판단하는 기준이 된다.** 간단하게 첫 번째 값을 soft threshold, 두 번째 값은 hard threshold라고 생각하면 된다. 결과적으로는 두번째 값에 정의된 횟수만큼을 넘겨야 실제 연결이 끊어진다.

# 9.3 재전송 추적하기

`tcpretrans` 스크립트를 통해 재전송 여부를 추적할 수 있다.

```docker
root@ip-172-31-0-62:/home/ubuntu# ./tcpretrans 
TIME     PID    LADDR:LPORT          -- RADDR:RPORT          STATE  
```

`tcpretrans` 스크립트를 살펴보면 1초에 한 번씩 깨어나서 `ftrace`를 통해 수집한 커널 함수 정보를 바탕으로 재전송이 일어났는지 아닌지를 파악한 후, /proc/net/tcp의 내용을 파싱해서 어떤 세션에서 재전송이 일어났는지를 출력한다.

RTO_MIN 값이 200ms기 때문에 1초의 인터벌은 트래픽이 많은 서버라면 재전송되는 패킷을 놓칠 수도 있다. 그래서 좀 더 정확한 추적이 필요하다면 interval 값을 200ms로 수정해서 실행시키는 방법을 취한다.

# 9.4 RTO_MIN 값 변경하기

RTO_MIN값이 200ms이기 때문에 아무리 RTT가 작은, 빠른 내부 통신의 경우에도 RTO 값은 200ms 밑으로 내려갈 수 없다.

```c
#define TCP_RTO_MAX ((unsigned)(120*HZ))
#define TCP_RTO_MIN ((unsigned)(HZ/5))
```

커널 소스 코드를 살펴보면 위와 같이 `TCP_RTO_MAX`, `TCP_RTO_MIN` 값을 define으로 정의했다. HZ의 경우 보통 1초이기 때문에 RTO의 최댓값은 120초, 최솟값은 200ms이다. RTO가 RTT를 기반으로 계산되지만 `TCP_RTO_MIN` 이 200이기 때문에 무조건 200보다는 커진다.

```c
cubic wscale:7,7 rto:202 rtt:1.18/0.056 ato:57 mss:1448 pmtu:9001 rcvmss:1448 advmss:8949 cwnd:10 bytes_sent:42975 bytes_acked:42975 bytes_received:15632 segs_out:295 segs_in:415 data_segs_out:282 data_segs_in:143 send 98.2Mbps lastsnd:37 lastrcv:38 lastack:36 pacing_rate 196Mbps delivery_rate 41.6Mbps delivered:283 app_limited busy:496ms rcv_space:17408 rcv_ssthresh:35328 minrtt:0.961 snd_wnd:56704
```

- rto: 현재 연결되어 있는 세션의 RTO 값이다. RTO는 RTT를 기반으로 생성되기 때문에 세션마다 별도의 RTO 값을 가지고 있다.
- rtt: 현재 연결되어 있는 세션의 RTT 값이다. 앞의 값은 RTT의 최댓값, 뒤에 있는 값은 측정된 RTT의 편차다. 즉, 패킷을 주고 받는 데에만 1.18ms의 시간이 걸리며 각각의 패킷은 편차 0.056ms 이내에서 값이 변동된다는 의미이다.

RTO의 값은 RTT를 기반으로 생성되며 `TCP_RTO_MIN` 값이 200이기 때문에 위의 예제를 보면 RTO가 202로 계산된 것을 볼 수 있다. 물론 RTO가 RTO_MIN + RTT_MAX라는 단순한 식은 아니지만, 얼추 그 정도 값이라고 추측해볼 수 있다.이 세션의 경우 RTT의 최댓값이 1.18ms에 편차가 0.056ms이고, 대부분의 패킷이 등락이 크지 않은 상태로 1.18ms 정도의 수준이라면 주고받을 수 있다는 것을 의미한다. 그런데 RTO가 202라면 너무 큰 것 같지 않은가? 비교적 긴 시간동안 기다리는 것은 오히려 더 낭비일 수 있다. 이 값을 바꿀 수는 없을까?

리눅스에 있는 `ip route` 라는 명령의 rto_min 옵션을 통해서 RTO의 최솟값을 `TCP_RTO_MIN` 보다 작게 바꿔줄 수 있다. 세션별로 바꿀 수는 없으며, 하나의 네트워크 디바이스를 기준으로 바꿀 수 있다.

```c
ip route change default via <GW> dev <DEVICE> rto_min 100ms
```

먼저 `ip route` 명령을 이용해서 현재 서버에 설정되어 있는 라우팅 정보를 확인한다.

```c
root@ip-172-31-0-62:/home/ubuntu# ip route
**default via 172.31.0.1 dev enX0** proto dhcp src 172.31.0.62 metric 100 
172.31.0.0/24 dev enX0 proto kernel scope link src 172.31.0.62 metric 100 
172.31.0.1 dev enX0 proto dhcp scope link src 172.31.0.62 metric 100 
172.31.0.2 dev enX0 proto dhcp scope link src 172.31.0.62 metric 100 
```

첫 번째 줄이 우리가 필요로 하는 정보다. 기본적으로 외부와의 통신을 위한 모든 패킷은 enX0이라는 네트워크 디바이스의 172.31.0.1 게이트웨이를 통해서 나간다는 의미이다.

```c
root@ip-172-31-0-62:/home/ubuntu# ip route change default via 172.31.0.1 dev enX0 rto_min 100ms
RTNETLINK answers: No such file or directory
root@ip-172-31-0-62:/home/ubuntu# sysctl -a 2>/dev/null | grep -i rto
net.ipv4.tcp_frto = 2
net.ipv4.tcp_plb_suspend_rto_sec = 60
net.ipv4.tcp_rto_min_us = 200000
net.netfilter.nf_conntrack_dccp_timeout_partopen = 480
root@ip-172-31-0-62:/home/ubuntu# sudo sysctl -w net.ipv4.tcp_rto_min_us=100
net.ipv4.tcp_rto_min_us = 100
```

- 우분투에서는 책처럼 변경하는 건 불가능하고 위 값을 바꿔줬다.
    
    ```c
             cubic wscale:7,7 rto:202 rtt:1.094/0.062 
    ```
    
    - 변화가 없다 ㅠㅠ

사실 rto_min이 어느 정도면 적당한가에 대한 답은 없다. 외부에 노출된 웹 서버에는 다양한 고객들이 접근하기 때문에 기본값으로 정해진 200ms를 따르는 것이 좋겠지만, 내부와 통신하는 서버에서는 200ms라는 값이 길게 느껴지는 것이 사실이다. 내부 통신의 rtt는 매우 짧기 때문에 좀 더 빠른 재전송이 필요한지 확인하고, rto_min 값을 그에 상응하는 수준으로 낮춰서 빨리 보내는 것이 서비스의 품질을 높일 수 있는 좋은 방법이다. 하지만 이 값이 너무 낮다면 너무 잦은 재전송이 일어날 수도 있기 때문에 신중해야 한다.

[네트워크 안정성 및 성능 향상을 위한 리눅스 TCP RTO 및 tcp_retries2 파라미터의 작동 원리](https://osslab.s-core.co.kr/232035f5-1294-80ad-b6f7-fd94179c5bed)

# 9.5 애플리케이션 타임아웃

TCP 재전송이 일어나면 애플리케이션에서는 어떤 일이 벌어질까? 요청한 내용을 전달받지 못했기 때문에 타임아웃이 발생한다. 하지만 타임아웃의 임계치를 몇 초로 설정했느냐에 따라 타임아웃이 발생할 수도, 발생하지 않을 수도 있다.

애플리케이션에서 발생할 수 있는 타임아웃에는 크게 두 종류가 있다. Connection Timeout과 Read Timeout이다.

| 종류 | 발생 경로 | 최소 권장 설정 값 |
| --- | --- | --- |
| Connection Timeout | TCP Handshake 과정에서 재전송이 일어날 경우 발생 | 3s 이상 |
| Read Timeout | 맺어져 있는 세션을 통해서 데이터를 요청하는 과정에서 발생 | 300ms 이상 |

## Connection Timeout

**Connection 과정에서 timeout이 발생했다는 의미로, 최초 TCP Handshake 과정에서 실패한 것이다.** 이는 **SYN 패킷 혹은 SYN+ACK 패킷 중 하나가 유실되어 재전송이 일어날 때 발생**할 수 있다. Handshake 과정에서 실패라고 했는데 왜 제일 마지막 ACK 패킷의 유실은 포함되지 않을까? 앞에서도 언급했지만, SYN, SYN+ACK 패킷은 종단에 대한 정보가 없기 때문에 RTO를 계산하기 위한 RTT 값을 구할 수가 없다. 그렇기 때문에 기본적으로 1초로 설정되어 있다. 하지만 **SYN과 SYN+ACK를 주고 받은 후에는 종단에 대한 정보가 생기기 때문에 해당 패킷에 대한 RTT 값을 측정할 수 있게 되고 이때부터는 RTO가 계산된다.** 그래서 Connection Timeout은 SYN, SYN+ACK의 유실에서 발생한다.

## Read Timeout

**읽기 작업 과정에서 타임아웃이 발생했다는 의미이다.** 이는 이미 연결되어 있는 세션을 통해서 데이터를 읽으려고 하다가 타임아웃이 발생했다는 것을 말한다. 주로 커넥션 풀 방식을 이용해서 특정 서버와 다수의 네트워크 세션을 만들어 놓은 상태에서 발생하는 타임아웃이다.

그럼 각각의 값은 어느 정도로 하는 것이 좋을까? 경우에 따라 다르겠지만 두 경우에 언제 발생하느냐를 이해해야 적절한 값을 설정할 수 있을 것이다. 만약 Connection Timeout을 1초로 설정하면 어떨까? SYN 혹은 SYN+ACK 패킷의 재전송은 무조건 1초 정도가 필요하다. 그렇다면 단 한 번의 SYN, SYN+ACK 패킷의 유실은 무조건 타임아웃을 일으킬 수밖에 없다. 한 번의 재전송 정도는 커버할 수 있도록 1초보다는 큰 값을 설정하는 것이 좋다. 재전송 일어나면 타이머는 1초가 지난 후에는 2초로 늘어나며 두 번의 재전송이 일어나게 되면 총 3초의 시간이 소요된다. 그렇기 때문에 두 번 이상의 재전송은 충분히 일어날 수 있지만 연달아 두 번 이상의 재전송이 일어나는 것은 문제가 있다는 뜻이기 때문이다. 따라서 Connection Timeout은 대체로 3초로 설정하는 것이 좋다. 왜냐하면 내가 보낸 SYN 재전송(1초)에 상대방의 SYN+ACK 재전송(1초)을 더한 2초보다 큰 값으로 설정해야 불필요한 타임아웃 에러 메시지를 줄일 수 있기 때문이다.

Read Timeout은 몇 초로 설정하는 것이 좋을까? 이미 맺어져 있는 세션에서 패킷을 읽어오는 경우에 타임아웃이 발생하기 때문에 Connection Timeout과 마찬가지로 한 번의 재전송 정도는 커버할 수 있는 값으로 설정해야 한다. 일반적으로 300ms 정도로 설정하는데 RTO_MIN 값이 200ms이고 맺어져 있는 세션이 재전송할 때 최소한 200ms 시간이 필요하기 때문이다. 만약 200ms보다 작은 값을 설정하면 단 한번의 재전송에서도 타임아웃 메시지가 나타나게 된다. 300ms 정도로 설정하면 한 번의 재전송은 커버할 수 있지만, 두 번 이상의 연속된 재전송이 일어나면 타임아웃 메시지를 출력해서 어딘가에 문제가 있음을 알려야 한다. 물론 통신을 연결하는 쪽이 어느 곳이냐에 따라 300ms 이상으로 설정해야 할 수도 있다. RTT가 길어서 RTO가 200ms 이상이라면 300ms 보다 더 크게 설정해야 한다. 하지만 대부분 내부 서버끼리의 통신일 경우가 많아서 RTT가 짧기 때문에 RTO가 200ms보다 커지는 경우는 없을 것이다.