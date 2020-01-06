'''

    Get the all lyrics of TOEFL's Listeing
    Save all files into *.txt & *.json

    @time: 5.12.2019
    @author: gltina

'''
import requests
import re
import json
import time

url_list = []
url_base = "http://*/"
menUrl_base = "http://*/tpo-list-{}.html"


def capture_url():
    maxTpoListIndex = 10
    for i in range(maxTpoListIndex+1):
        url = menUrl_base.format(i)
        r = requests.get(url)
        if r.status_code == 200:
            res = re.findall(r"(play/(\d{3,5}).html)", r.text)
            for j in range(len(res)):
                url_list.append({'url': url_base+res[j][0], 'title': ''})
            print("第{}页url获取完毕".format(i))
        else:
            print("连接失败")
    print("完成，共收集到{}个url".format(len(url_list)))


def convert2sentence (lyric_json):
    paragraph_list = lyric_json['paragraph']
    lyric_list = []
    for i in range(len(paragraph_list)):
        for j in range(len(paragraph_list[i]['content'])):
            content = paragraph_list[i]['content'][j]
            # print(content['start'], content['en'])
            lyric_list.append(str(content['start'])+' '+content['en']+'\n')
    return lyric_list


def save_lyric(url_item):
    r = requests.get(url_item['url'])
    if r.status_code == 200:
        res = re.search(r'datalist(\W*)\{([\d\w\s\D\W\S]*)\}\}\]\}', r.text)
        lyric_json = json.loads(r.text[res.start():res.end()].replace("datalist =", "").strip())
        title = lyric_json['title']
        url_item['title'] = title.split(' ')[0].replace('-', '')+title.split(' ')[1]
        with open(url_item['title']+'.json', 'w', encoding='utf-8') as f:
            json.dump(lyric_json, f)
        print("保存为 {}".format(url_item['title']+'.json'))

        lyric_list = convert2sentence(lyric_json)
        with open(url_item['title']+'.txt', 'w', encoding='utf-8') as f:
            f.writelines(lyric_list)
        print("保存为 {}".format(url_item['title'] + '.txt'))
    else:
        print("保存失败")


if __name__ == '__main__':
    capture_url()
    print("正在下载lyric...")
    for i in range(len(url_list)):
        print("正在下载第{}个".format(i))
        save_lyric(url_list[i])
        # print('({}/{})Saved:{}'.format(i+1, len(url_list), url_list[i]['title']))
        print("({}/{})第{}个下载完成".format(i,len(url_list),i))
        print("暂停一会儿")
        time.sleep(0.5)
    print("完成下载lyric")
