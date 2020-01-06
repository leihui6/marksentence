## 1. 排序（用于点击table的head排序）
给定一组数据，对其从小到大排序

```
要求：
1. 不能改变原来数组顺序
2. 结果给出有序索引
```

```
案例：
input: 0 1 2 0 1 2 0 1 2
output: 0 3 6 1 4 7 2 5 8
NOTE:对稳定性没有要求
```
### My solution

``` C++
void MainWindow::sortby(...){
// 把要排序的放到一个新数组里
// note:直接排序会破坏索引
QVector<int> value_seq;
for (int i = 0;i<m_json_vec.size();i++){
    value_seq.push_back(m_json_vec[i][key].toInt());
}
// 有点类似冒泡思路，需要一个个比较
for (int i = 0; i<value_seq.size();i++){
    // 找一个有效值当基准
    for (j=0;j <value_seq.size();j++){
        if (value_seq[j] != -1){
            num = value_seq[j];
            index = j;
            break;
        }
    }
    //找到匹配值，找到最小值
    for (int n = 0;n < value_seq.size();n++){
        if (value_seq[n]  != -1 && value_seq[n] < num){
            num = value_seq[n];
            index = n;
        }
    }
    value_seq[index]  = -1;
    result_vec.push_back(index);
	}
}
```

## Analysis 
时间复杂度:`O(n*n)`


## 2. 给出合适的数（用于歌词的显示）
给定一个整数序列，接着，在给定一个随机数，指出所给定随机数在序列中那两个数之间。

```
案例：
input：[4 9 10 12 15 18 20 22 23] 19
output:[20,22]

input：[4 9 10 12 15 18 20 22 23] 1
output:[null,4]
```

### My solution
```C++
int index = 0;
for (QMap<int,QString>::iterator it = m_lyric_map.begin();
     it != m_lyric_map.end(); ++it,++index) {
    if (m_music.position() < it.key()){
        if (index-2 >= 0) {
            ui->textBrowser_1->setText((it-2).value());
        }
        if (index-1 >= 0) {
            ui->textBrowser_2->setText((it-1).value());
        }
        if (index < m_lyric_map.size()) {
            ui->textBrowser_3->setText((it).value());
        }
        break;
    }// end if
}// end for
```

### Analysis 
None


## 3. 分享一个有意思的话题
由于音频播放的时候，会发出一个信号，信号内容包含了当前播放的位置，比如`20 26 29 35 ... `这样的信号，你也发现了，他们并不是那么的连续，甚至有可能较大的离散间隙，如`1 10 900`，那么如何才能在音频播放到`506`的时候执行指定动作呢？ 例如我想让音频在播放到`506`位置的时候返回到`50`的位置继续播放！ 没错，这就是这个小工具中的部分功能。

其实实现的方法很简单，随时获取播放的位置，如果播放的位置等于`506`不就行了？ 恐怕确实不行，因为间隙的不确定，很难遇到碰巧等于的情况，更多的可能是小于`506`或者大于`506`的情况。 那怎么解决这个问题呢？或者说是看似解决这个问题呢？ 
1. 提高发送信号的频率，使之尽可能发送诸如`1 2 3 4 5 6`这样的数据。
2. 使用大于操作来**挽回**

第二种方案也就是只要收到的信号中的位置大于等于所要求的位置就算"等于"了。

在[mainwindow.cpp](https://github.com/Gltina/marksencent/blob/master/mainwindow.cpp)中的解决方法就是：
``` C++
if (m_music.position() >= m_end_point+1/*+1防止在结束时返回的技巧*/){
        m_music.setPosition(m_beg_point);
    }
```

同时也使用了提高发送频率的方法：
`m_music.setNotifyInterval(100); // 听说人类的最快反应就是100ms` 


