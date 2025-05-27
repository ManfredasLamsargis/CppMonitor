To build a project run build script (requires CMake).
For linux:
```bash
./scripts/build.sh
```
Binaries are in the build directory:
```bash
./build/Monitor
```
Expected console output:
```txt
------------------------------------------------------------
[05/27/25 15:35:34:062][36084][info    ]: example1 started
[05/27/25 15:35:34:263][36088][debug   ]: reader: {b}
[05/27/25 15:35:35:244][36088][debug   ]: reader: {c}
[05/27/25 15:35:35:758][36088][debug   ]: reader: {a}
[05/27/25 15:35:36:208][36088][debug   ]: reader: {a}
[05/27/25 15:35:36:569][36088][debug   ]: reader: {b}
[05/27/25 15:35:37:120][36088][debug   ]: reader: {b}
[05/27/25 15:35:37:839][36088][debug   ]: reader: {a}
[05/27/25 15:35:38:522][36088][debug   ]: reader: {b}
[05/27/25 15:35:39:207][36088][debug   ]: reader: {a}
[05/27/25 15:35:39:808][36088][debug   ]: reader: {a}
[05/27/25 15:35:40:378][36088][debug   ]: reader: {b}
[05/27/25 15:35:40:907][36088][debug   ]: reader: {c}
[05/27/25 15:35:40:907][36084][info    ]: example1 completed
------------------------------------------------------------
[05/27/25 15:35:40:907][36084][info    ]: example2 started
[05/27/25 15:35:41:207][36150][debug   ]: consumer: 4 => {producer:1}
[05/27/25 15:35:41:689][36150][debug   ]: consumer: 4 => {producer:0}
[05/27/25 15:35:42:088][36150][debug   ]: consumer: 4 => {producer:0}
[05/27/25 15:35:42:140][36147][debug   ]: consumer: 1 => {producer:2}
[05/27/25 15:35:42:595][36146][debug   ]: consumer: 0 => {producer:2}
[05/27/25 15:35:42:629][36149][debug   ]: consumer: 3 => {producer:1}
[05/27/25 15:35:43:808][36084][info    ]: example2 completed
------------------------------------------------------------
```