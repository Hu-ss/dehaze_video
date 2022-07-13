# A fast dehazing method for video based on recursive filtering
一种基于递归滤波的视频快速去雾方法，视频去雾的主要步骤是：
1. 通过视频序列中一定帧数的图像提取视频的背景图像；
2. 利用背景图像快速求出视频的大气光强与大气透射率；
3. 将求出的大气光强与大气透射率运用到计算背景图像的视频序列的去雾当中。
# 有雾视频
<div align=center>
  <img src=https://user-images.githubusercontent.com/80933553/178755625-f9a68d81-6fc7-45a9-a6d8-3232a43a359d.jpg width = 500 />
</div>

# 去雾视频
<div align=center>
  <img src=https://user-images.githubusercontent.com/80933553/178755591-6314c0b8-f154-432f-8cc3-b890365d40af.jpg width = 500 />
</div>
