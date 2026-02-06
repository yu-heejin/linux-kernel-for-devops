[TCP 통신 중 TIME_WAIT 개념](https://usheep91.tistory.com/61)

# 7.1 TCP 통신 과정

**통신을 시작하기 전에 최초의 연결을 맺게 되는 과정을 3-way handshake라고 한다.** 클라이언트는 서버로 통신을 시작하겠다는 SYN을 보내고, 서버는 그에 대한 응답으로 SYN+ACK을 보낸다. 마지막으로 클라이언트는 서버로부터 받은 패킷에 대한 응답으로 ACK을 보낸다. 이렇게 3-way handshake를 정상적으로 마친 다음 클라이언트는 서버에 데이터를 요청한다. **통신을 모두 마친 후에는 연결을 종료한다. 이 과정을 4-way handshake**라고 한다.

연결을 맺을 때는 연결을 맺고자 하는 쪽에서 먼저 SYN을 보내며, 연결을 끊을 때는 연결을 끊으려는 쪽에서 먼저 FIN을 보낸다. 클라이언트는 서버의 FIN을 받고 ACK을 보내고, 사용한 소켓을 정리한 다음 마지막 FIN을 보낸다. 서버는 클라이언트가 보낸 마지막 FIN에 대한 ACK을 전송하고 소켓을 정리한다.

<img width="2048" height="447" alt="image" src="https://github.com/user-attachments/assets/7eb932b6-1bc9-41de-a389-8afbecf84df5" />

- 데이터를 받고, 클라이언트는 ACK을 보낸다.
- 위 이미지는 클라이언트가 먼저 끊었다.
- FIN과 ACK을 같이 보낸 이유는 앞선 패킷에 대한 ACK 응답과 연결을 끊기 위한 FIN 응답을 같이 보냄

# 7.2 TIME_WAIT 소켓의 문제점

연결을 끊는 과정을 조금 더 자세히 살펴보자. 그림을 보면 active closer, passive closer가 있는데 단어 그대로 먼저 연결을 끊는 쪽을 active closer라고 하고 그 반대를 passive closer라고 한다. 누가 먼저 연결을 끊느냐가 중요한 이유는 active closer 쪽에 TIME_WAIT 소켓이 생성되기 때문이다. 주의해야 할 부분은 **TIME_WAIT 소켓은 서버에서 생기는 것이 아니고 먼저 연결을 끊는 쪽에서 생성된다는 점이다.** 

그럼 서버에서 TIME_WAIT 소켓이 몇개나 있는지 어떻게 확인할 수 있을까? `netstat` 명령으로 확인할 수 있다.

```docker
root@ip-172-31-0-128:/home/ubuntu# netstat -napo |grep -i time_wait
tcp        0      0 172.31.0.128:33870      13.124.245.230:80       TIME_WAIT   -                    timewait (41.94/0/0)
```

출발지 IP가 172.31.0.128:33870이며, 목적지 IP가 13.124.245.230:80인 소켓이 있는데 현재 TIME_WAIT 상태이기 때문에 **타이머가 종료되어 커널로 다시 돌아갈 때까지는 사용할 수 없다.**

이처럼 TIME_WAIT 소켓이 많아지면 어떤 문제가 발생할까? 먼저 로컬 포트 고갈에 따른 애플리케이션 타임아웃이 발생할 수 있다. 리눅스에는 `net.ipv4.ip_local_port_range`라는 커널 파라미터가 있는데, 이 파라미터는 외부와 통신하기 위해 필요한 로컬 포트의 범위를 지정하는 역할을 한다. 커널은 프로세스가 외부와 통신하기 위해 소켓의 생성을 요청할 때 해당 소켓이 사용하게 될 로컬 포트에 `net.ipv4.ip_local_port_range`에 정의된 값들 중 하나를 넘겨준다. 이때 모든 로컬 포트가 TIME_WAIT 상태에 있다면 할당할 수 있는 로컬 포트가 없기 때문에 외부와 통신을 하지 못하게 되고, 이로 인해 애플리케이션에서는 타임 아웃이 발생할 수 있다.

그리고 잦은 TCP 연결 맺기/끊기로 인해 서비스의 응답 속도 저하도 일어날 수 있다. TIME_WAIT 소켓은 어찌 되었든 연결을 끊기 때문에 발생하는 것인데, 지속적으로 통신량이 많을 때도 연결의 맺고 끊기를 반복한다면 그만큼 많은 양의 TCP 3-way handshake가 필요하게 되고 이는 전체적인 서비스의 응답 속도 저하를 야기할 수 있다. 이런 현상을 막기 위해 대부분의 애플리케이션에서는 Connection Pool과 같은 방식을 사용해서 한 번 맺어 놓은 TCP 연결을 계속해서 재사용할 수 있게 구현하고 있다. 이를 통해서 불필요한 TCP 3-way handshake를 줄일 수 있어서 성능 향상에도 도움이 된다.

# 7.3 클라이언트에서의 TIME_WAIT

TIME_WAIT은 서버에 생기는 것이 아니라 먼저 연결을 끊는 쪽에서 생성된다. HTTP 기반의 서비스는 대부분 서버가 먼저 연결을 끊는 경우가 많기 때문에 서버에서 TIME_WAIT가 생긴다고 오해할 수 있지만 그렇지 않다.

그럼 클라이언트 입장에서의 TIME_WAIT는 어떻게 발생할 수 있을까? 대부분의 시스템들은 독립적으로 동작하지 않는다. 데이터 저장 및 가공을 위해 데이터베이스, 메모리 기반의 캐시 시스템들과 연동하기도 하고 외부 서비스와의 연동을 위해 API를 호출하기도 한다. 이런 과정에서 서비스를 제공하는 서버는 연동하는 시스템에 대해서는 클라이언트가 될 수 있다.

그림과 같은 과정을 가정해보자. 사용자는 POST 메소드를 이용해 웹 서버에 데이터를 업로드하고 데이터를 받은 웹 서버는 DB 서버에 해당 데이터를 저장한다. 1번 과정을 보면 클라이언트는 USER, 서버는 WEB SERVER다. 하지만 2번 과정을 보면 클라이언트는 WEB SERVER, 서버는 DB SERVER가 된다. 즉, 통신하는 과정에 따라 서버의 역할을 했던 서버는 반대로 클라이언트 역할을 하기도 한다. 그리고 이 과정에서 클라이언트의 역할을 하는 서버가 먼저 연결을 끊는다면 클라이언트 입장의 TIME_WAIT 소켓이 발생할 수 있다.

클라이언트 입장에서 TIME_WAIT가 발생했을 때 가장 큰 문제는 로컬 포트가 고갈되는 것이다. 클라이언트는 요청을 보내기 위해 소켓을 만드는데, 이때 가용한 로컬 포트 중 하나를 임의로 배정 받아서 나가게 된다.

그림과 같은 상황을 가정해보자.

1. 애플리케이션은 DB 서버와의 통신을 위해 커널에 소켓 생성을 요청한다.
2. 커널은 자신이 관리하고 있는 로컬 포트 목록 중에 사용할 수 있는 포트 번호 한 개를 애플리케이션에 할당한다. 
3. 애플리케이션은 할당 받은 번호로 커널에 소켓 생성을 요청한다.
4. 커널은 해당 정보로 소켓을 생성한다. 소켓은 출발지 IP, 출발지 Port, 목적지 IP, 목적지 Port 이 4개의 값을 한 묶음으로 해서 생성하며 해당 소켓은 커널 내부에 유일하게 존재한다. 즉, 4개의 값이 동일한 또 다른 소켓은 존재하지 않는다.
5. 소켓 생성이 정상적으로 완료되면 커널은 애플리케이션에서 소켓 접근에 사용할 FD(File Descriptor)를 전달해준다.

이렇게 사용된 소켓을 active close 하게 되면 TIME_WAIT 상태로 남는다. 그렇기 때문에 정상적인 상황이라면 해당 소켓은 TIME_WAIT 상태가 풀려서 커널로 다시 돌아갈 때까지 다시 사용할 수 없다. 이런 식으로 다량의 로컬 포트가 TIME_WAIT 상태로 쌓이고 더 이상 사용할 수 있는 포트가 없어지면 로컬 포트가 고갈되며 서버와 통신할 수 없게 된다.

```docker
root@ip-172-31-0-128:~# curl http://www.kakao.com > /dev/null
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
  0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0
root@ip-172-31-0-128:~# netstat -napo | grep 80
tcp        0      0 0.0.0.0:80              0.0.0.0:*               LISTEN      575/nginx: master p  off (0.00/0/0)
tcp        0      0 172.31.0.128:56792      169.254.169.254:80      TIME_WAIT   -                    timewait (11.29/0/0)
tcp        0      0 172.31.0.128:56692      169.254.169.254:80      TIME_WAIT   -                    timewait (10.60/0/0)
tcp        0      0 172.31.0.128:56776      169.254.169.254:80      TIME_WAIT   -                    timewait (11.28/0/0)
                                            # 카카오 요청
tcp        0      0 172.31.0.128:39894      121.53.93.60:80         TIME_WAIT   -                    timewait (47.09/0/0)
tcp        0      0 172.31.0.128:80         193.142.147.209:43876   SYN_RECV    -                    on (26.33/5/0)
tcp        0      0 172.31.0.128:56948      169.254.169.254:80      TIME_WAIT   -                    timewait (12.25/0/0)
tcp        0      0 172.31.0.128:56666      169.254.169.254:80      TIME_WAIT   -                    timewait (10.29/0/0)
tcp        0      0 172.31.0.128:56970      169.254.169.254:80      TIME_WAIT   -                    timewait (12.29/0/0)
tcp        0      0 172.31.0.128:58424      169.254.169.254:80      TIME_WAIT   -                    timewait (2.18/0/0)
tcp        0      0 172.31.0.128:58660      169.254.169.254:80      TIME_WAIT   -                    timewait (2.24/0/0)
```

[AWS의 메타데이터 서버가 링크 로컬 주소를 사용하는 이유 ( 169.254.169.254 주소는?)](https://everenew.tistory.com/469)

위 결과를 보면 www.kakao.com에 HTTP 프로토콜로 GET 요청을 할 때 이 요청을 처리할 소켓이 필요한데, (172.31.0.128:39894, 121.53.93.60:80)을 하나의 쌍으로 만들어서 나갔다. 이것은 TIME_WAIT 상태가 풀릴 때까지 동일한 목적지 IP, 동일한 목적지 포트를 사용할 수 없다는 뜻이다.

```docker
root@ip-172-31-0-128:~# sysctl -w "net.ipv4.ip_local_port_range=39894 39894"
net.ipv4.ip_local_port_range = 39894 39894
root@ip-172-31-0-128:~# curl http://www.kakao.com > /dev/null
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
  0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0
root@ip-172-31-0-128:~# curl http://www.kakao.com > /dev/null
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
  0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0
**curl: (7) Failed to connect to www.kakao.com port 80 after 11 ms: Couldn't connect to server**
```

위 결과를 보면 첫 번째 요청에서 이미 39894를 이용해 나갔으며, 39894를 사용한 소켓은 TIME_WAIT 상태이기 때문에 다음번에 다시 사용할 수 없다. 이런 식으로 외부로의 요청에 TIME_WAIT 소켓이 쌓이면 더 이상 할당할 수 있는 로컬 포트가 없어서 사용자의 요청을 처리할 수 없게 된다.

# 7.4 net.ipv4.tcp_tw_reuse

첫 번쨰로 로컬 포트 고갈에 대응할 수 있는 방법은 커널 파라미터를 이용하는 방법이다. TIME_WAIT 소켓을 처리하는 커널 파라미터 중 **`net.ipv4.tcp_tw_reuse`는 외부로 요청할 때 TIME_WAIT 소켓을 재사용할 수 있게 해준다.** 조건은 아까와 같이 `net.ipv4.local_port_range` 를 39894 39894로 고정시키고 `net.ipv4.tcp_tw_reuse` 값을 1로 설정한 다음 이전 절에서의 테스트와 마찬가지로 `curl` 명령을 두 번 연달아서 입력한다.

```docker
root@ip-172-31-0-128:~# sysctl -w "net.ipv4.tcp_tw_reuse=1"
net.ipv4.tcp_tw_reuse = 1
root@ip-172-31-0-128:~# sysctl -w "net.ipv4.ip_local_port_range=39894 39894"
net.ipv4.ip_local_port_range = 39894 39894
root@ip-172-31-0-128:~# curl http://www.kakao.com > /dev/null
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
  0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0
root@ip-172-31-0-128:~# curl http://www.kakao.com > /dev/null
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
  0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0
```

이전 절에서의 테스트 결과와 달리 에러 메시지 없이 잘 실행된다.

그럼 `net.ipv4.tcp_tw_reuse`는 어떤 방식으로 동작하게 되는걸까?

그림을 보면 kernel은 `net.ipv4.local_port_range` 범위 안에서 임의의 값을 선택한 다음 TW Socket Array에 해당 값을 사용하는 동일한 쌍의 소켓이 있는지 확인한다. 이때 `net.ipv4.tcp_tw_reuse` 값이 켜져 있으면 해당 값을 사용하도록 그대로 리턴하고, 꺼져 있으면 다른 값을 선택해서 확인하는 과정을 다시 진행한다. 이를 통해서 `net.ipv4.tcp_tw_reuse`를 사용하면 TIME_WAIT 상태의 소켓을 재사용해서 외부로 요청을 보낸다.

# 7.5 ConnectionPool 방식 사용하기

앞 절에서 `net.ipv4.tcp_tw_reuse`를 사용하면 TIME_WAIT 상태의 소켓을 재사용할 수 있다는 것을 확인했다. 이제 로컬 포트가 고갈되어 발생하는 장애는 처리할 수 있다. 하지만 좀 더 근본적인 문제 해결 방법이 있다.

앞에서도 언급했지만 TIME_WAIT 소켓이 쌓이는 문제는 active close 때문에 발생한다. 즉, 먼저 연결을 끊기 때문에 TIME_WAIT 소켓이 발생하고, 이후의 통신을 위해서 다시 연결을 맺어야 해서 리소스 낭비가 발생한다. 그럼 연결을 먼저 끊지 않으면 되지 않을까? 이럴때 Connection Pool 방식의 접근 방법을 사용한다.

그림을 보면 클라이언트의 동작 방식은 크게 두 가지로 나눌 수 있다. 첫 번째 Connection Less 방식은 HTTP가 많이 사용하는 방식으로, 요청할 때마다 소켓을 새로 연결하는 방식이다. 두 번째 Connection Pool 방식은 미리 소켓을 열어놓고 요청을 처리하는 방식이다. 미리 열어놓기 때문에 불필요한 TCP 연결 맺기/끊기 과정이 없어서 더 좋은 애플리케이션 응답 속도를 구현할 수 있다.

connection_less 코드를 실행시키고 나면 TIME_WAIT 소켓이 1초 단위로 생성되는 것을 볼 수 있다. 하지만 connection_pool 스크립트를 실행시키고 나면 시간이 흘러도 하나의 EST 소켓만 존재한다.

몇개의 요청을 처리할 때는 큰 차이가 없겠지만 초당 수십에서 수백 개의 요청이 들어오는 대규모의 시스템이라면 분명히 응답 속도에 영향이 있다.

Connection Pool 방식은 이렇게 로컬 포트의 무분별한 사용을 막을 수도 있고, 서비스의 응답 속도도 향상시킬 수 있기 때문에 가능한 한 사용하는 것이 좋다. 하지만 Connection Pool 방식도 단점이 있다.

# 7.6 서버 입장에서의 TIME_WAIT 소켓

서버쪽에서 보면 클라이언트와는 상황이 조금 다르다. 서버는 소켓을 열어 놓고 요청을 받아들이는 입장이기 때문에 로컬 포트 고갈과 같은 문제는 일어나지 않는다. 하지만 클라이언트와 마찬가지로 다수의 TIME_WAIT 소켓이 있으면 불필요한 연결 맺기/끊기의 과정이 반복된다. 어떤 경우에 서버에서 TIME_WAIT가 생길 수 있을까?

```docker
ubuntu@ip-172-31-0-13:~$ netstat -napo | grep -i :80
(No info could be read for "-p": geteuid()=1000 but you should be root.)
tcp        0      0 0.0.0.0:80              0.0.0.0:*               LISTEN      -                    off (0.00/0/0)
tcp        0      0 172.31.0.13:80          121.157.155.227:54504   TIME_WAIT   -                    timewait (49.48/0/0)
tcp        0      0 172.31.0.13:80          121.157.155.227:54511   TIME_WAIT   -                    timewait (50.56/0/0)
tcp        0      0 172.31.0.13:80          121.157.155.227:54508   TIME_WAIT   -                    timewait (50.06/0/0)
tcp        0      0 172.31.0.13:80          121.157.155.227:54505   TIME_WAIT   -                    timewait (47.99/0/0)
tcp        0      0 172.31.0.13:80          121.157.155.227:54510   TIME_WAIT   -                    timewait (50.38/0/0)
tcp        0      0 172.31.0.13:80          121.157.155.227:54507   TIME_WAIT   -                    timewait (49.90/0/0)
tcp        0      0 172.31.0.13:80          121.157.155.227:54509   TIME_WAIT   -                    timewait (50.24/0/0)
```

keepalive를 껐기 때문에 웹 서버가 먼저 연결을 끊는다. 즉, 웹서버가 active close 했기 때문에 웹 서버에서 TIME_WAIT 소켓이 생긴다.

![image.png](attachment:578a8d66-cc0a-488d-a1c2-9d639f219c51:image.png)

응답 헤더를 살펴보면 Connection: close라는 헤더가 내려온 것을 볼 수 있다. 이 헤더가 내려오면 먼저 연결을 끊는다는 의미이다.

# 7.7 net.ipv4.tcp_tw_recycle

`net.ipv4.tcp_tw_reuse`와 이름은 비슷하지만 전혀 다른 동작 로직을 가진 `net.ipv4.tcp_tw_recycle` 파라미터를 살펴보도록 하자.

앞 절에서 `net.ipv4.tw_reuse`는 나갈 때 사용하는 로컬 포트에서 TIME_WAIT 상태의 소켓을 재사용할 수 있게 해주는 파라미터라고 설명했다. `net.ipv4.tw_recycle`은 그 반대로 서버 입장에서 TIME_WAIT 상태의 소켓을 빠르게 회수하고 재활용할 수 있게 해주는 파라미터이다. 아래와 같이 파라미터를 수정할 수 있다.

```docker
sysctl -w "net.ipv4.tcp_tw_recycle=1"
```

서버에서 `netstat`을 입력하면 TIME_WAIT 소켓이 하나도 없는 것을 확인할 수 있다. 앞 절에서는 다수의 TIME_WAIT 소켓이 있었는데 거짓말처럼 TIME_WAIT 소켓이 줄어들었다. 왜 그런걸까?

`tcp_tw_recycle` 값이 커지면 커널은 아래와 같은 두 가지 작업을 추가로 진행한다.

1. 가장 마지막에 해당 소켓으로부터 들어온 timestamp 저장
2. TIME_WAIT 소켓의 타이머를 RTO 기반의 값으로 변경

<aside>
💡

TIME_WAIT 소켓의 기본 타임아웃은 1분이다.

</aside>

특히, 2번 과정 때문에 TIME_WAIT 소켓이 눈에 보일 새도 없이 사라진다. RTO는 보통 ms 단위이기 때문이다. 하지만 1번 과정 때문에 서비스에 문제가 생길 가능성이 있다.

C1과 C2가 동일한 통신사를 사용하는 클라이언트라고 가정해보자. 동일한 통신사를 사용하면 동일한 NAT을 사용할 수 있고, S 입장에서는 같은 목적지 IP를 달고 오기 때문에 같은 클라이언트로 보게 된다. 같은 클라이언트가 출발지 포트만 다르게 해서 요청하는 것과 같다. S는 C1과의 통신을 잘 마무리하고 로직상에 구현되어 있는 대로 TIME_WAIT 소켓을 RTO 값으로 세팅해서 금방 정리하고, C1의 Timestamp를 저장한다. 그 후 C2가 다시 한 번 연결 오픈 요청을 보내는데, **이때 C1과 C2는 동일한 클라이언트가 아니기 때문에 시간이 살짝 다를 수 있으며 이때 Timestamp 값이 C1이 보낸 FIN에 기록된 Timestamp보다 작을 수 있다.** S 입장에서는 동일한 IP를 가진 목적지에서 기존보다 더 작은 Timestamp를 가지고 통신 연결을 요청하기 때문에 **잘못된 연결 요청으로 판단하고 패킷을 처리하지 않고 버린다.** 하지만 C2는 패킷이 버려진 것을 모르고 재전송한다. 자신이 보낸 SYN에 대한 응답이 오지 않았기 때문이다. 이렇게 **연결은 되지 않고 연결 요청만 계속해서 일어나게 되는 현상이 클라이언트의 요청을 직접 받는 웹 서버에서 주로 발생할 수 있기 때문에 웹 서버에서는 절대로 `tw_recycle`을 켜서는 안된다.**

응, **네가 잘못 입력한 건 아니고** 👉 **커널이 그 옵션을 아예 없애버려서** 그런 거야.

핵심만 정리해줄게.

---

## 왜 에러가 난 거야?

```
sysctl: cannot stat /proc/sys/net/ipv4/tcp_tw_recycle: No such file or directory
```

이 메시지는 **오타**가 아니라

👉 **커널에 해당 파라미터 파일 자체가 존재하지 않는다**는 뜻이야.

### 이유

`net.ipv4.tcp_tw_recycle`은

- **Linux 커널 4.12 이후 완전히 제거됨**
- Ubuntu 18.04 이후(20.04, 22.04, 24.04 전부 포함)에서는 **존재 자체가 안 함**

그래서:

- `tcp_tw_recycle`
- `tw_recycle`

👉 둘 다 **없는 게 정상**

---

## 왜 제거됐냐면 (중요)

`tcp_tw_recycle`은 NAT 환경에서 **치명적인 문제**를 만들었어.

- 클라이언트 IP가 NAT 뒤에 있으면
- TIME_WAIT 소켓 재활용 시
- **정상 TCP 연결이 랜덤하게 끊김**
- 특히 **LB, 프록시, 클라우드 환경에서 대형 사고 유발**

그래서 커널 개발자들이:

> “이건 위험해서 튜닝 옵션으로 두면 안 된다”
> 
> 
> → **통째로 삭제**
> 

---

## 그럼 요즘은 어떻게 해야 해?

### 1️⃣ 이건 아직 있음 (안전)

```bash
net.ipv4.tcp_tw_reuse = 1
```

```bash
sudo sysctl -w net.ipv4.tcp_tw_reuse=1
```

- **클라이언트 측 outgoing connection**에만 영향
- 서버에서 써도 비교적 안전
- 실무에서도 허용되는 튜닝

---

### 2️⃣ TIME_WAIT 줄이고 싶으면 이걸 봐야 함

(요즘은 이쪽이 정공법)

- Keep-Alive 설정
- 애플리케이션 커넥션 재사용
- LB / 프록시 커넥션 풀
- HTTP/2, gRPC 같은 multiplexing

👉 **커널 튜닝으로 해결하려는 발상 자체가 옛날 방식**

# 7.8 keepalive 사용하기

TIME_WAIT 소켓을 완전히 없앨 수는 없지만 줄일 수 있는 방법이 있다. 바로 keepalive이다. keepalive는 한번 맺은 세션을 요청이 끝나더라도 유지해주는 기능이다. 예를 들어 2초 간격으로 계속해서 GET 요청이 들어온다면 2초마다 한 번씩 세션을 맺기 보다는 하나의 세션을 연결해놓고 그 연결을 유지하면서 지속적으로 요청을 처리하는 것이 서버 리소스 활용 면에서도 경제적이고, 서비스 응답 속도의 측면에서도 훨씬 도움이 된다. 불필요한 연결 맺기/끊기를 없애면 TPS(단위 시간당 대상 시스템에서 처리되고 있는 요청 건수)가 높아질수록 성능이 더 좋아진다.

[TPS와 응답시간](https://performance.tistory.com/4)

![image.png](attachment:9a77a241-a4e9-44ea-9f80-614a20362cff:image.png)

위 결과를 보면 Connection:close가 내려왔고, HTTP 클라이언트의 역할을 하는 telnet (나는 http를 썼음..)은 해당 헤더를 읽고 연결을 끊는다. 그리고 서버에서 확인하면 TIME_WAIT 소켓이 생성되어 있음을 볼 수 있다.

코드의 2번을 보면 외부에서 들어온 80 포트에 대한 요청이 TIME_WAIT 상태가 되었다. 172.16.33.137이 요청할 내용이 더 있다고 해도 서버가 먼저 끊기 때문에 요청마다 새로운 소켓을 열어야 한다. 만약 10번의 HTTP 요청이 있다면 저 TIME_WAIT 소켓은 10개가 될 것이다. keepalive를 켜서 TIME_WAIT 소켓의 생성을 줄일 수 있다면 불필요한 TCP 연결 맺기/끊기 작업이 없어지면서 응답 속도가 빨라진다.

```docker
keepalive_timeout 10;
```

![image.png](attachment:80a31272-76cb-4ddc-ad75-52da555a7b45:image.png)

서버의 nginx를 reload하고 클라이언트에서 연결하면 연결이 끊어지지 않는다. 계속해서 명령을 입력할 수 있도록 연결이 유지되어 있으며, 그 후로 계속 GET 요청을 보내도 끊어지지 않는다. keepalive의 타임아웃이 10초로 설정되어 있기 때문에 10초가 지나야만 서버에서 먼저 연결을 끊게 된다.

이런 식으로 서버의 입장에서 keepalive를 켜서 세션을 유지해주면 TIME_WAIT 소켓을 줄일 수 있으며, 불필요한 TCP 연결 맺기/끊기 과정이 사라지기 때문에 서비스의 응답 속도도 더 향상시킬 수 있다.

# 7.9 TIME_WAIT 상태의 존재 이유

**TIME_WAIT 소켓의 핵심은 연결이 종료된 후에도 소켓을 바로 정리하지 않고 일종의 연결 종료에 대한 흔적을 남겨 놓는 것에 있다.** 일정 시간 동안 연결 종료에 대한 흔적을 남겨 놓는 것에 있다 일정 시간 동안 연결 종료에 대한 흔적을 남겨서 발생할 수 있는 문제점을 방지하는 것이 TIME_WAIT 소켓의 핵심인 것이다. 이때 발생할 수 있는 가장 큰 문제점은 패킷 유실에 따른 비정상적인 통신 흐름의 발생이다. 그리고 그 중에서도 필자의 개인적인 생각으로는 연결 해제 시 발생할 수 있는 문제를 방지하는 것이 TIME_WAIT 소켓의 가장 큰 필요성이라고 생각한다.

그림의 (a)와 같이 TIME_WAIT 상태가 매우 짧다고 가정해보자, 서버는 FIN을 먼저 보내서 클라이언트와의 통신을 종료한다. 헌데 서버에서 마지막으로 보낸 ACK이 중간에 유실되어 클라이언트에서는 자신이 보낸 FIN에 대한 ACK을 받지 못한 상황이 된다. 클라이언트의 입장에서는 ACK을 받지 못했기 때문에 FIN을 다시 한 번 보내게 되지만, 서버의 입장에서는 이미 TIME_WAIT 상태의 소켓을 정리해 버렸기 때문에 클라이언트로부터 받은 FIN이 정상적인 FIN 이라 판단하지 않고 RST를 보낸다. 클라이언트는 자신이 정리해야 하는 소켓에 대해 계속해서 정상적인 ACK을 받지 못하기 때문에 소켓이 LAST_ACK 상태로 계속해서 남아있게 된다. 그래서 비정상적으로 LAST_ACK 상태의 소켓이 점점 증가할 수 있다.

반대로 (b)와 같이 TIME_WAIT 상태가 정상적이라면, 서버의 ACK가 유실되었다고 하더라도 클라이언트의 재전송된 FIN을 처리할만한 충분한 시간 동안 소켓이 TIME_WAIT 상태로 살아있기 때문에 자신이 ACK을 보냈다고 하더라도 무언가 통신에 이상이 생겼음을 감지하고 C의 FIN에 대해 한 번 더 ACK을 보내게 된다. 즉 패킷 유실로 인해 발생한 FIN과 ACK의 재전송을 처리할 수 있는 기회를 얻게 되는 것이다.

이런 이유 때문에 TCP에서는 연결을 끊은 이후에도 일정 시간 동안 소켓을 유지하고 있으며 이때의 소켓 상태를 TIME_WAIT 상태로 정의하는 것이다.
