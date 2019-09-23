---
theme: research
paginate: true
---
<!-- _class: cover -->
# 輪読会 #1
# Attacking Network Protocols #9
The Root Causes of Vulnerabilities
ed

---
# The Root Causes of Vulnerabilities
- 脆弱性の根本原因は？
  - プロトコル実装のミス
  - プロトコル設計ミス(#7で議論済み)
- 脆弱性があるからといってExploit可能とは限らない
  - プロトコルのセキュリティを低下させる要因にはなる
  - プロトコルへの攻撃が容易に
  - 他の深刻な脆弱性につながる
- 本章で達成すること
  - 脆弱性分析のパターンを知る
  - 実際のExploitは10章

---
# Vulnerability Classes(1/2)
- 脆弱性を分類しておくといいことがある
  - 脆弱性の悪用(Exploit)によって生じるリスクを評価しやすくなる
- Remote Code Execution(Critical)
  - アプリケーションのコンテキスト内で任意のコードが実行できる脆弱性
  - アプリロジックのハイジャック, サブプロセス生成時のコマンドライン引数の操作
- Denial-of-Service
  - アプリケーションをクラッシュ, 応答不可状態にする脆弱性
  - 一般的に，DoSの達成は少ないリソースで可能
  - 1パケットでアプリケーションを落とす
  - Persistent｜管理者が復旧し問題を解決するまでサービスダウン
    データストアにクラッシュさせる要因を仕込まれるため
    サービス再起動だけでは対応できない場合が多い
  - Nonpersistent｜攻撃者がデータを送信し続ける間のみサービスダウン
    アプリ自身が再起動可能なシステムなら再起動で復旧可能

---
# Vulnerability Classes(2/2)
- Information Disclosure
  - 想定しない情報を引き出される脆弱性
  - メモリダンプ, ファイルシステムパス, 認証証明書
  - 攻撃者にとってremote code executionなど次の攻撃に有用な情報
- Authentication Bypass
  - 認証をバイパス可能な脆弱性
  - アプリケーションはユーザ認証によって, 認証されていないユーザのアクセス権の範囲を制限する
    ユーザ名, パスワードの組み合わせなどで認証
  - 認証がパスワードのチェックサムの比較で実装されているとブルートフォースで突破可能
- Authorization Bypass
  - 認証されたユーザがより上位の権限が必要なファイルにアクセスできる脆弱性
  - 認証されたユーザ情報や権限を直接変更
  - プロトコルのユーザ権限のチェックが不適切

---
# Memory Corruption Vulnerabilities
- メモリ汚染は脆弱性分析で最も遭遇しやすく様々な問題につながる
  - 不正なメモリ情報はアプリケーションのクラッシュを引き起こす(DoS)
  - Remote Code Execution
- Memory-Safe vs. Memory-Unsafe Programming Languages
  - メモリの脆弱性はプログラミング言語に大きく依存する
  - Memory-Safe Languages(Java, C#, Python, Ruby)
    開発者は低レベルなメモリ管理をしない
    ライブラリがunsafeな操作を提供する場合もある(C#`unsafe`)
    -> 明示的に利用し, 言語的に安全性をチェック
  - Memory-Unsage Languages(C, C++)
    メモリアクセスの検証やロバスト機構がなく脆弱性が埋め込まれやすい
    メモリ汚染の対策はいろいろ工夫している(10章)

---
# Memory Corruption Vulnerabilities
- Memory Buffer Overflows
  - 固定長バッファオーバーフロー
    入力バッファがアロケートされたバッファサイズにフィットすると思いこむと発生
  - 可変長バッファオーバーフロー
    バッファサイズの計算ミスで発生

    ![img width:800px](../img/attcking_network_protocols_09/buffer-overflow.png)

---
# 固定長バッファオーバーフロー
- バッファはコンパイル時にスタック, ヒープ, グローバル領域に静的に割り当て
  - inputの長さチェックが脆いとオーバーフロー
  ```
  def read_string()
  {
    byte str[32];
    int i = 0;
    do
    {
      str[i] = read_byte();
      i = i + 1;
    }
    while(str[i-1] != 0); printf("Read String: %s\n", str);
  }
  ```
  * 入力の長さを未確認(コンパイラによるメモリ破損対策がなければ簡単に悪用される)
  * -> カイゼン

---
# 固定長バッファオーバーフロー
- 入力の長さチェックを導入
  - まだ脆弱性が存在
    ```
    def read_string_fixed()
    {
      byte str[32];
      int i = 0;
      do
      {
        str[i] = read_byte();
        i = i + 1;
      }
      while((str[i-1] != 0) && (i < 32));

      /* Ensure zero terminated if we ended because of length */
      str[i] = 0;
      printf("Read String: %s\n", str);
    }
    ```
* str[i] = 0; // i = 32(off-by-one エラー)

---
# 可変長バッファオーバーフロー
- バッファ長の計算ミスにより発生
    ```
    def read_uint32_array()
    {
      uint32 len;
      uint32[] buf;
      // Read the number of words from the network
      len = read_uint32();
      // Allocate memory buffer
      buf = malloc(len * sizeof(uint32));
      // Read values
      for(uint32 i = 0; i < len; ++i)
      {
        buf[i] = read_uint32();
      }
      printf("Read in %d uint32 values\n", len);
    }
    ```
    * 脆弱性が存在
---
# 整数オーバーフロー
- 整数演算は値の範囲内に収まるように計算される(モジュロ演算)
![img width:800px](../img/attcking_network_protocols_09/int_overflow.png)
* len の値を適切に設定することで, mallocの引数値を小さくすることが可能
  * lenとバッファサイズの不整合によりオーバーフロー

---
# Out-of-Bounds Buffer Indexing
- 配列の範囲外参照は選択的なメモリ読み込み/書き込みの脆弱性につながる
    ```
    byte app_flags[32];
    def update_flag_value()
    {
      byte index = read_byte();
      byte value = read_byte();
      printf("Writing %d to index %d\n", value, index);
      app_flags[index] = value;
    }
    ```
  * 範囲チェックが存在しないため脆弱
  * 範囲外データが読み取られたものを出力すれば情報漏洩
- 関数ポインタ配列を保持しindexで次の関数を決定するような処理の場合
  - indexを範囲外の適切な位置に設定(攻撃者が容易に変更可能なアドレス帯を値としてもつ場所)
  - 攻撃者がremote code executionのためにコードを配置した場所を実行
---
# Data Expansion Attack
- 最新の高速ネットワークでもデータを圧縮しネットワークに流れるオクテット数を減らす
  - データ転送時間を減らしてパフォーマンスを改善するか帯域幅コストを削減するかのどちらかを達成
  - 途中で解凍する必要がある
    ```
    void read_compressed_buffer()
    {
      byte buf[];
      uint32 len;
      int i = 0;
      // Read the decompressed size
      len = read_uint32();
      // Allocate memory buffer
      buf = malloc(len);
      gzip_decompress_data(buf)
      printf("Decompressed in %d bytes\n", len);
    }
    ```
  * 送信された元データの本当のサイズをチェックしないためバッファオーバーフローの可能性あり

---
# Dynamic Memory Allocation Failures
- メモリ割り当て関数はメモリプールが不足するとエラーが返される
- これを適切に処理する実装でない場合クラッシュしDoSとなりうる

---
# Default or Hardcoded Credentials
- 認証を用いる場合, インストール時にデフォルトの認証情報が設定されており, 管理者が利用開始前にアカウント情報を再構成(怠ると脆弱性につながる)
- アプリにハードコードされた認証情報が残っていた場合より深刻な脆弱性
  - デバッグのために追加されたもの, 意図的に設置されたバックドア
    ```
    def process_authentication()
    {
      string username = read_string();
      string password = read_string();
      // Check for debug user, don't forget to remove this before release
      if(username == "debug")
      {
        return true;
      } else {
        return check_user_password(username, password);
      }
    }
    ```
  - 実際は送信元IPが制限されており, マジックナンバーの送信くらいは必要

---
# User Enumeration
- ユーザ名, パスワードのセットによる認証はよくありユーザ名は一般的に開示情報であり非公開にされてはいない
  - 有効なユーザ一覧を開示するのは大丈夫なのか？？
    ```
    def process_authentication()
    {
      string username = read_string();
      string password = read_string();
      if(user_exists(username) == false)
      {
        write_error("User " + username " doesn't exist");
      }
      else
      {
        if(check_user_password(username, password))
        {
          write_success("User OK");
        } else {
          write_error("User " + username " password incorrect"); }}}
    ```
  * 有効なユーザを知られるとブルートフォース攻撃を受けやすくなるため開示しない

---
# Incorrect Resource Access
- httpやファイル共有プロトコルはリソースをパスで指定するが, 適切に処理しない場合想定しない場所へアクセスされる脆弱性となる
- Canonicalization｜パスの正規化における脆弱性
    ```
    def send_file_to_client()
    {
      string name = read_string();
      // Concatenate name from client with base path
      string fullPath = "/files" + name;
      int fd = open(fullPath, READONLY);
      // Read file to memory
      byte data[] read_to_end(fd);
      // Send to client
      write_bytes(data, len(data));
    }
    ```
  * pathに".."が含まれたら/files以外を見られるため連結前にパスのチェックが必要
  * OS毎に実装が異なる(Windowsは区切りに/,\を両方使えるため/のみのチェックでは不十分)
---
# Incorrect Resource Access
- システムからダウンロードなら情報漏洩のみだが, アップロード時はより深刻
  - システムに悪意ある実行可能コードを配置し, システムにコードを実行させる
![img width:400px](../img/attcking_network_protocols_09/incorrect_access.png)

---
# Incorrect Resource Access
- Verbose Errors
  - エラーメッセージが最小限でなく, 必要以上の情報を与える脆弱性
    ```
    def send_file_to_client_with_error()
    {
      string name = read_string();
      // Concatenate name from client with base path 􏰂string fullPath = "/files" + name;
      if(!exist(fullPath)) {
      write_error("File " + fullPath + " doesn't exist"); }
      else
      {
        write_file_to_client(fullPath);
      }
    }
    ```
  * ベースパスの場所を開示すると攻撃者がアクセスしたいリソースへの参照方法が決定する
  * リソースがホームディレクトリにあるとアプリを実行している現在のユーザがバレる

---
# Memory Exhaustion Attacks
- メモリリソースを無駄に使わせることでシステムをダウンさせる攻撃
  - プロトコルで受信する値をもとにメモリを割り当てるコードはまずい
    ```
    def read_buffer()
    {
      byte buf[];
      uint32 len;
      int i = 0;
      // Read the number of bytes from the network
      len = read_uint32();
      // Allocate memory buffer
      buf = malloc(len);
      // Allocate bytes from network
      read_bytes(buf, len);
      printf("Read in %d bytes\n", len);}
    ```
  * 攻撃者が2GBのメモリを簡単に指定可能, 確保後ゆっくりデータを送信(タイムアウト防止)を何度か繰り返すとメモリ枯渇
  * 仮想メモリがある場合はほぼ，実際にメモリを使うまで物理メモリは確保されないため影響は少ない
  * 組み込みシステムなどのメモリが貴重で仮想メモリが存在しない場合は深刻

---
# Storage Exhaustion Attacks
- ストレージを枯渇させる攻撃
  - 組み込みなどのストレージが小さいシステムにとっては問題
  - 枯渇すると他のユーザでエラーが発生したり, システムの再起動さえ妨げられる
    OSは起動前に特定のファイルをディスクに書き込む必要がある場合があり, 失敗すると起動しない
- ディスクへの動作情報のロギングが原因になりがち
  - 最大ログサイズの制限がない場合, サービスに繰り返し接続することでログで枯渇可能
  - 圧縮データをサポートしているとより深刻

---
# CPU Exhaustion Attacks
- 多くのシステムはマルチコアだが一度に実行可能なタスク数には制限がある
- CPUの枯渇はDoSを引き起こす
- Algorithmic Complexity
  - アルゴリズムの計算量の多さによってCPUが枯渇しうる
  - バブルソートは正順のデータセットでO(n), 逆順のデータセットでO(n^2)
  - 攻撃者が逆順のデータセットを指定できる場合DoSになる
  - 実例｜JavaやPHPなどで最悪の場合O(N^2)操作をするハッシュテーブル実装が存在
    - エントリ挿入時にキーが衝突すると発生した
    - 攻撃者は数個のパケット送信でDoSを達成

---
# CPU Exhaustion Attacks
- Configurable Cryptography
  - 攻撃への耐性を持たせるため, パスワードのハッシュ化はハッシュチェーンを通すがハッシュ処理は案外重い
    ```
    def process_authentication()
    {
      string username = read_string();
      string password = read_string();
      int iterations = read_int();
      for(int i = 0; i < interations; ++i)
      {
        password = hash_password(password);
      }
      return check_user_password(username, password);
    }
    ```
  * interations を大きな値にすることでDoS

---
# Format String Vulnerabilitie
- ...
    ```
    def process_authentication()
    {
       string username = read_string();
       string password = read_string();
       // Print username and password to terminal
       printf(username);
       printf(password);
       return check_user_password(username, password))
    }
    ```
  * username = "%04x%0x4%04x..."
  * printf()の引数はスタックに積まれているため, 下位のスタックメモリダンプが可能

---
# Command Injection
- ...
    ```
    def update_password(string username)
    {
      string oldpassword = read_string();
      string newpassword = read_string();
      if(check_user_password(username, oldpassword))
      {
      // Invoke update_password command
      system("/sbin/update_password -u " + username + " -p " + newpassword);
      }
    }
    ```
  * newpassword = aaaa; /bin/sh

---
# SQL Injection
- ...
    ```
    def process_authentication()
    {
      string username = read_string();
      string password = read_string();
      string sql = "SELECT password FROM user_table WHERE user = '" + username + "'";
      return run_query(sql) == password;
    }
    ```
  * username = "aaa' AND password = 'password' UNION ALL SELECT password FROM user_table WHERE password = 'password' '"
  * 認証バイパス

---
# Text-Encoding Character Replacement
- ...
    ```
     def add_user()
    {
      string username = read_unicode_string();
      // Ensure username doesn't contain any single quotes
      if(username.contains("'") == false)
      {
        // Add user, need to convert to ASCII for the shell
        system("/sbin/add_user '" + username.toascii() + "'");
      }
    }
    ```
  * シェルが解釈できるようにasciiに変換するが, unicodeよりasciiはbit数が小さく情報が欠落する可能性がある
