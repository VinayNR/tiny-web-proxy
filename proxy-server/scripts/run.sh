curl -x "http://127.0.0.1:7777" http://httpbin.org/get --verbose

curl -x "http://127.0.0.1:7777" http://netsys.cs.colorado.edu/images/welcome.png --verbose
curl -x "http://127.0.0.1:7778" http://netsys.cs.colorado.edu/images/welcome.png --verbose
curl http://netsys.cs.colorado.edu/images/welcome.png --verbose --OUTPUT welcome2.png
curl -x "http://127.0.0.1:7777" http://netsys.cs.colorado.edu/images/welcome.png --verbose --OUTPUT welcome.png
curl -x "http://127.0.0.1:7777" http://netsys.cs.colorado.edu/images/apple_ex.png --verbose --OUTPUT apple_ex.png
curl -x "http://127.0.0.1:7777" http://netsys.cs.colorado.edu/images/wine3.jpg --verbose --OUTPUT wine3.jpg
curl -x "http://127.0.0.1:7777" http://netsys.cs.colorado.edu/images/exam.gif --verbose --OUTPUT exam.gif
curl -x "http://127.0.0.1:7777" http://netsys.cs.colorado.edu/files/text1.txt --verbose --OUTPUT text1.txt
curl -x "http://127.0.0.1:7778" http://netsys.cs.colorado.edu/images/welcome.png --verbose --OUTPUT test.png
curl http://netsys.cs.colorado.edu/images/welcome.png --verbose --OUTPUT test.png
curl -x "http://127.0.0.1:7778" http://httpbin.org/get --verbose
curl -x "http://admin:admin@127.0.0.1:7777" http://httpbin.org/get --verbose
curl -x "http://127.0.0.1:7777" http://www.example.com/ --verbose

curl http://httpbin.org/get --verbose
curl http://localhost:7777/admin/dnscache --verbose
curl http://en.wikipedia.org/wiki/Web_server --verbose
curl -x "http://127.0.0.1:7778" http://netsys.cs.colorado.edu/ --verbose
curl -x "http://127.0.0.1:7777" http://netsys.cs.colorado.edu/ --verbose
curl -x "http://127.0.0.1:7778" http://netsys.cs.colorado.edu/graphics/mov.gif --verbose
curl -x "http://127.0.0.1:7777" http://netsys.cs.colorado.edu/files/text1.txt --verbose
curl -x "http://127.0.0.1:7778" http://netsys.cs.colorado.edu/files/text1.txt --verbose
curl -x "http://127.0.0.1:7777" http://netsys.cs.colorado.edu/images/apple_ex.png --verbose
curl -x "http://127.0.0.1:7777" http://alive.github.com:443 --verbose