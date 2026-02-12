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

이렇게 특정 프로세스에 할당되지 않고 커널에 귀속되어 정리되기를 기다리는 소켓 중에서도 FIN_WAIT1 상태의 소켓을 orphan socket이라고 하며, 이 값으로 재전송하는 횟수를 정의한다. 왜 FIN_WAIT2와 TIME_WAIT는 아니고 FIN_WAIT1만 해당될까?