# A fast dehazing method for video based on recursive filtering
一种基于递归滤波的视频快速去雾方法。该方法的主要步骤是：
1. 通过视频序列中一定帧数的图像提取视频的背景图像；
2. 利用背景图像快速求出视频的大气光强与大气透射率；
3. 将求出的大气光强与大气透射率运用到计算背景图像的视频序列的去雾当中。

## 有雾视频
<div align="center">
   <img src="https://user-images.githubusercontent.com/80933553/178894951-ce8c91d4-34f3-4abe-9deb-001743252e89.jpg" width=200 height=160> <img src="https://user-images.githubusercontent.com/80933553/178894958-8ee759e3-14b8-412d-91be-585ea28dfe80.jpg" width=200 height=160>
   </div>
<div align="center">
   <font color="AAAAAA">视频图像1</font>
	 &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
	 <font color="AAAAAA">视频图像2</font>
   </div>
<div align="center">
   <img src="https://user-images.githubusercontent.com/80933553/178894964-76476698-f7fc-4d34-9076-1c9b92f89258.jpg" width=200 height=160> <img src="https://user-images.githubusercontent.com/80933553/178894969-9a06cc5e-b7df-481b-a49b-d31cbb5b1586.jpg" width=200 height=160>
   </div>
<div align="center">
   <font color="AAAAAA">视频图像3</font>
	 &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
	 <font color="AAAAAA">视频图像4</font>
   </div>

## 去雾视频
<div align="center">
   <img src="https://user-images.githubusercontent.com/80933553/178895338-34b8a4d2-fcd9-477e-a67b-23c186f09738.jpg" width=200 height=160> <img src="https://user-images.githubusercontent.com/80933553/178895350-80eed791-e051-48d3-bba8-9b431cfd6d70.jpg" width=200 height=160>
   </div>
<div align="center">
   <font color="AAAAAA">视频图像1</font>
	 &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
	 <font color="AAAAAA">视频图像2</font>
   </div>
<div align="center">
   <img src="https://user-images.githubusercontent.com/80933553/178895356-ec1bc459-b34b-44ce-99ac-73d57f17368b.jpg" width=200 height=160> <img src="https://user-images.githubusercontent.com/80933553/178895362-3d0c9b8f-1d92-4309-8fae-3323728df1e6.jpg" width=200 height=160>
   </div>
<div align="center">
   <font color="AAAAAA">视频图像3</font>
	 &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
	 <font color="AAAAAA">视频图像4</font>
   </div>
