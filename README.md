# SplatoonKillDigestMaker

## 環境
- Visual Studio 2015
- OpenCV 3.4.1　https://opencv.org/releases.html
- FFmpeg 20180608-2bd26de　https://ffmpeg.zeranoe.com/builds/
- Avidemux 2.7.1 Win64 https://ja.osdn.net/projects/avidemux/

## 各設定
### Visual Studio 2015
- C++
### OpenCV
- Cドライブ直下に展開後「opencv3.4.1」に改名
### FFmpeg
- 任意の場所に置き，「bin」の中身のパスを登録しておけばたぶん大丈夫．
- コマンドプロンプトかWindows PowerShellで「ffmpeg -version」と打ち込んでバージョンが出てくればOK
### Avidemux
- 任意の場所に置き，「avidemux_cli.exe」がある場所のパスを登録　→　再起動
- コマンドプロンプトかWindows PowerShellで「avidemux_cli」と打ち込んでわらわら何かが出てくればOK

## 使い方
- 「TargetFiles」というディレクトリを「Source.cpp」と同じ階層に作成．その中に動画ファイルを入れる．
