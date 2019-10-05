# Serial4Unreal
シリアル通信を行うブループリントノードを追加するプラグインです。

## USerial4UnrealBPLibrary (ブループリントファンクションライブラリ)

- Get Com Port List (関数)
   : 接続されたデバイスの情報とCOM番号を取得します。

- Create Serial Port (関数)
   : シリアル通信を行うためのクラスを作成します。
  
## USerialPort (シリアル通信を行うクラス)

- Open (関数)
   : 指定されたCOM番号のポートを開きます。
  
- Close (関数)
   : 現在開いているポートを閉じます。
  
- Write (関数)
   : 指定した文字列を送信します。
   
- Read (関数)
   : 受信バッファーに溜まっている文字を1文字取り出します。
   
- GetAmoutOfDataReceived (関数)
   : 受信バッファーに溜まっているデータの量を取得します。
   
- bIsOpen (変数)
   : 現在ポートが開かれているかを表します。
   
- OnDataRecived (イベントディスパッチャー)
   : 受信バッファーにデータが追加された時に呼び出されます。
   
- OnReceptionCompleted (イベントディスパッチャー)
   : 受信バッファーに全てのデータが送られてた時に呼び出されます。
