# Paper_Counting_Display_Device

Project for Embedded Advanced Experiment.

## 实验要求

+ 设计并制作纸张计数显示装置

+ 两块平行极板（极板 A、 极板 B）分别通过导线 a 和导线 b 连接到测量显示电路，装置可测量并显示置于极板 A 与极板 B 之间的纸张数量。

  ![image-20210722113756654](纸张计数实验报告\image-20210722113756654.png)

+ 基本要求

  +  极板 A 和极板 B 上的金属电极部分均为边长 50mm±1 mm 的正方形， 导线 a 和导线 b 长度均为 500mm±5mm。测量显示电路应具有“自校 准”功能，即正式测试前，对置于两极板间不同张数的纸张进行测量， 以获取测量校准信息。
  +  测量显示电路可自检并报告极板 A 和极板 B 电极之间是否短路。
  +  测量置于两极板之间 1~10 张不等的给定纸张数。每次在极板间放入被测纸张，一键启动测量，显示被测纸张数并发出一声蜂鸣。 每次测量从按下同一测量启动键到发出蜂鸣的时间不得超过 5 秒钟， 在此期间对测量装置不得有任何人工干预。

+ 发挥部分

  + 极板、导线均不变，测量置于两极板之间 15~30 张不等的给定纸张数。 对测量启动键、显示蜂鸣、测量时间、不得人工干预等。

  + 极板、导线均不变，测量置于两极板之间 30 张以上的给定纸张数。 对测量启动键、显示蜂鸣、测量时间、不得人工干预等。


## 实验设备

+ PC机
+ keil5
+ STM32F103RCT6核心板
+ FDC2214模块
+ OLED模块
+ $5cm\times5cm$两片极板
+ 屏蔽双绞线
+ 亚克力板
+ 绝缘体重物



## 实验原理　　

### FDC2214模块

​	选用FDC2214测量两极板间电容，为高分辨率（28位）、多通道的电容数字转换器。与传统的开关电容架构相比，此芯片使用LC谐振器作为传感器，窄带架构可以实现高抗电磁干扰能力，并大大降低噪声。

#### 变间隙式电容传感器

​	两极板构成一变间歇式电容传感器，在上下极板放入纸张，产生微小移动，即相应改变板间距d，电容量C与极板间距d的关系式如下所示：
$$
C=\frac{\epsilon_0\epsilon_rA}{d}
$$
​	其中$\epsilon_0$是真空中的介电常数，$\epsilon_r$是复印纸的相对介电常数，A是极板面积，我们采用的是$5cm\times5cm$单面覆铜板，又因为板间距d与纸张数N成正比，故纸张数N与电容量C存在反函数关系。

#### LC谐振

​	LC谐振电路包括并联电路和串联电路两种结构类型，电感L、电容C和外加信号源u组成的并联谐振电路如图所示。其中r是电感L的损耗电阻，电容的损耗一般忽略不计。

<img src="纸张计数实验报告\image-20210722153027898.png" alt="image-20210722153027898" style="zoom:25%;" />

​	在该电路中，随着储能在电场和磁场之间的往返转移，电路中的电流和电压将不断地改变大小和极性，形成周而复始的振荡。当振荡持续维持时，称之为等幅振荡，也称为谐振。LC电路的谐振频率f为
$$
f=\frac{1}{2\pi \sqrt{LC}}
$$
​	则对于LC谐振器，电容的变化可体现为谐振频率的变化。

#### FDC2214测量原理与计算

​	FDC2214外接电容传感器极板，主要由谐振电路驱动器、多路复用器、内核和I2C接口组成，其功能框图如图所示。在芯片每个检测通道的输入端首先连接一个初始电感和电容，组成LC电路，被测电容传感端与该电路相连接，共同构成一个谐振电路，随着待测电容的变化，其输出的谐振频率$f_{SENSOR}$也会发生变化。在芯片内部，谐振电路驱动器通过多路复用器连接到测量和数字化传感器频率$f_{SENSOR}$的核心，该内核使用一个参考频率fCLK去测量传感器频率，最终每个通道的输出与$\frac{f_{SENSORRE}}{f_{REF}}$成比例关系。I2C接口用于与MCU通信。传感器在差分配置下，待测电容CSENSOR可由下式计算得到下式：
$$
C_{SENSOR}=\frac{1}{L\times(2\pi\times f_{SENSOR})^2}-C
$$
​	其中，C是与待测电容并联的初始电容值。而FDC2214传感器频率$f_{SENSOR}$与输出数字量DATA具体的关系式为：
$$
f_{SENSOR}=\frac{CH\_FIN\_SEL\times f_{REF}\times DATA}{2^28}
$$
​	其中CH_FIN_SEL是传感器频率$f_{SENSOR}$​的分频系数，$f_{REF}$​是参考时钟$f_{CLK}$​分频后的信号频率。又CSENSOR与纸张数N成反比，可得
$$
(k_1\frac{1}{N}+C)\times L=k_2\times\frac{1}{f_{REF}^2\times DATA^2}
$$
​	其中$k_1、k_2$均为大于零的常数。

​	根据已校准的点及原始出厂数据，由斜率拟合出所有新的校准点，存入数组`FDC2214CalibrateValue[MaxPaperNum]`中，即可计算出纸张数量。

以下为`纸张数-厚度`图：

<img src="纸张计数实验报告\image-20210726201006838.png" alt="image-20210726201006838" style="zoom:30%;" />




### OLED模块

​	OLED（Organic Light-Emitting Diode），又称为有机电激光显示、有机发光半导体（Organic Electroluminescence Display，OLED）。OLED属于一种电流型的有机发光器件，是通过载流子的注入和复合而致发光的现象，发光强度与注入的电流成正比。OLED在电场的作用下，阳极产生的空穴和阴极产生的电子就会发生移动，分别向空穴传输层和电子传输层注入，迁移到发光层。当二者在发光层相遇时，产生能量激子，从而激发发光分子最终产生可见光。
