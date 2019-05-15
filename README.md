# Reading

## Pages Link
https://daisuke-settai.github.io/Reading/

## Edit(参考程度に)
- ページ作成
  1. masterからブランチを切る: git checkout -b "本のタイトル#章の番号"
  2. 適当に編集(git-pagesはローカルでサンプル表示できるよ．後述)
  3. commitが汚れていたら整理: git rebase -i HEAD~[纏めたいコミットまでの個数]
  4. git push origin [ブランチ名]
  5. 適当にマージ

- 新規輪読追加
  1. 新規ディレクトリ作成
  2. 新規輪読本のIndexページ作成(テンプレート: /sample/index.md)
  3. メインIndexページ編集(新規輪読本のIndexページへのリンクをはる)


## Others
- ローカルでサンプルページ表示
  (rubyとgemは入ってるよね？)
  1. gem install bundler (bundler 入ってますか？)
  2. ./Reading ディレクトリに移動
  3. bundle install (jekyll 導入するよ)
  4. bundle exec jekyll server (ローカルにサーバー立てるよ)
  [Reference](https://sakanasoft.net/github-pages-by-localhost/)
