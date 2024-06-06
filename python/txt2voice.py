from pathlib import Path
import pyttsx3

# 输入文件
input_file = Path('content.txt')

# 输出文件夹
output_dir = Path('out')


def process_voice():
    # 读入文件
    with open(input_file, 'r', encoding='utf8') as f:
        lines = f.readlines()
    
    # 初始化TTS引擎
    tts = pyttsx3.init()
    tts.setProperty('rate', 150)
    tts.setProperty('volume', 1.0)
    voices = tts.getProperty('voices')
    for voice in voices:
        print(voice)

    for line in lines:
        line = line.replace('\n', '')
        line = line.replace('\r', '')
        # 分割每行
        info = line.split('\t')
        no = info[0]
        text = info[1]

        # 建立输出文件夹
        outdirname = 'DM' + ('000' + no)[-3:]
        outdir = output_dir.joinpath(outdirname)
        if not outdir.exists():
            outdir.mkdir(parents=True)

        # 输出文件夹路径
        outfile = outdir.joinpath('1.mp3')

        # 转换为语音保存至mp3文件中
        print(text, outfile)
        tts.save_to_file(text, str(outfile))
        tts.runAndWait()


    # 输出文件夹路径
    outfile = output_dir.joinpath('init.mp3')
    text = '欢迎使用必利优科技的RGV小车'
    # 转换为语音保存至mp3文件中
    print(text, outfile)
    tts.save_to_file(text, str(outfile))
    tts.runAndWait()


if __name__ == "__main__":
    process_voice()
