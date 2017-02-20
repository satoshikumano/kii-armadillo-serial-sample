# kii-armadillo-serial-sample

Armadillo-IoTゲートウェイ上で動作するサンプルアプリケーションです。
シリアルから入力された文字列をKii Cloudにアップロードします。


## 要件
- Armadillo-IoTゲートウェイ<br>
このサンプルアプリケーションの動作確認には「Armadillo-IoTゲートウェイ G2」を使用しました。

- RS232Cアドオンモジュール<br>
(「Armadillo-IoTゲートウェイ G2」を利用する場合には標準で搭載されています。)

- ATDE<br>
このサンプルアプリケーションの動作確認にはATDE5を使用しました。

- Windowsマシン(仮想マシンでも可)<br>
このサンプルアプリケーションの動作確認にはWindows10を使用しました。

- シリアルクロスケーブル<br>
このサンプルアプリケーションの動作確認には以下のケーブルとD-sub9ピンをメスに変換するアダプタを利用しました。<br>
iBUFFALO USBシリアルケーブル(USBtypeA to D-sub9ピン)0.5m ブラックスケルトン BSUSRC0605BSk

## 準備

### Tera Termのインストール
WindowsマシンにTeraTermをインストールします。

### Armadillo-IoT の起動

デバッグシリアルケーブルとWindowsマシンを接続します。<br>
[4.4章](http://manual.atmark-techno.com/armadillo-iot/armadillo-iotg-std_product_manual_ja-2.8.0/ch04.html#sct.connect-method)参照。

Tera Termからデバッグシリアルポートに接続しArmadilo-IoTをbootします。<br>
[5章](http://manual.atmark-techno.com/armadillo-iot/armadillo-iotg-std_product_manual_ja-2.8.0/ch05.html)
を参考にしてください。

Armadillo-IoTが起動したらrootでログインしておきます。

#### IPアドレスの確認

ifconfig を実行しArmadillo-IoTに割り当てられたIPアドレスを確認しておきます。
```shell
ifconfig
```

### ATDE のインストール

[第４章](http://manual.atmark-techno.com/armadillo-iot/armadillo-iotg-std_product_manual_ja-2.8.0/ch04.html)
を参考にしてATDEをインストールしてください。

(このチュートリアルではATDEからminicomを利用してArmadillo-IoTへの接続は必須ではありません。)

#### ソースコードの取得

atmark のユーザーでログインしてホームディレクトリに作業用ディレクトリを作成します。


```shell
mkdir work
cd work
```

Githubからソースコードを取得します

```shell
git clone https://github.com/satoshikumano/kii-armadillo-serial-sample.git
```

#### Kii Cloudのアプリケーション設定
