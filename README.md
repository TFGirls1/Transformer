# Transformer

## Tokenization

考虑到计算机没有办法直接识别人类语言，我们将每一个词映射为一个token使得计算机可以直接识别。
为实现这个目的我们使用BPE算法将每个词划分为若干个前缀和后缀，以此拼起来一个词，节省vocabulary占用的空间。
这一部分写于BPE.h

## Word2Vec

现在我们把语言tokenize以后，我们希望得到词汇的词向量，以完成后续的embedding操作。
我们考虑如何处理词向量。首先一个比较简略的办法是one-hot表示法，有且仅有一个元素是1，其他全部置0，然后每一个单词的词向量都不相同。但是这样空间利用率不够高，并且也没有足够的区分度，所以我们考虑去使用一种distributed的表示法，让每一个维度都表示一个该词的属性，所有维度拼起来，就是这个词的全部意义。

所以我们现在补充一下前置知识，对于机器学习，我们评估它的训练质量是使用极大似然估计和损失函数来进行的。

e.g. 如果一个袋子里有若干个红球，若干个蓝球，取10次球，取到的蓝球个数为$x$，我们得到一个结果$E(x) = 7$，那么我们该怎么反推单次取蓝球的概率？
一个比较直观的想法是我们去让$P(x = 7)最大$。

也就是说，我们令$P(x = 7) = \binom{10}{7} p^7(1-p)^3$最大，其中$p$是单次取蓝球的概率。

由于是指数级的，我们取对数$\log P(x = 7) = \binom{10}{7}(7\log p + 3 \log (1-p))$

令其导数为0，即$7 \frac{1}{p} - 3 \frac{1}{1-p} = 0$，得到p = 0.7

推广一下，如果说现在有若干种颜色的球，也就是有若干种不同的可能性，每种可能称为$D_i$，且$D_i$满足伯努利分布。

那么我们考虑$P(X) = \binom{\sum_{i=1}^{n}|D_i|}{|D_1|,|D_2|,|D_3|,...,|D_n|}\prod_{i = 1}^{n}P(D_i)$

由于组合数是一个固定的常数，求导后置0的过程中与它的值无关，而且有的时候事情的发生是有顺序的，不需要乘上组合数的系数，我们大可以忽略掉。

因此我们可以定义$\log P(X) = \sum_{i = 1}^n \log (D_i) = \sum_{i = 1}^n (\log pD_i + \log(1-p) (1-D_i))$

由于我们要令这个值最大，所以也就是使得它的负值最小，于是$-\log p = -\sum_{i = 1}^n (\log pD_i + \log(1-p) (1-D_i))$定义为它的交叉熵损失函数，我们训练的时候要最小化这个函数的值。

不过在上面的例子中我们可以直接得出来p在最大值点的取值，依旧是求导。

$\sum_{i = 1}^n (D_i \frac{1}{p} - (1 - D_i) \frac{1}{1 - p})$

化简得：$p = \frac{1}{n}\sum_{i = 1}^nD_i$

另一个需要引入的概念是梯度下降法。

假设有一个目标函数，$F(x_1,x_2,...,x_n)$，我们想要令这个目标函数逐渐趋向于最大值，我们就可以设定一个参数$\eta$称为学习率，我们对每一个自变量求梯度，并$x_i := x_i + \eta\frac{\partial F}{\partial x_i}$，最终就可以趋向于最大值。

### CBOW

该模型的目的是根据上下文估算当下词语，学习目标是最大化它的对数似然函数。

$\mathcal{L} = \sum_{w \in \mathcal{C}}\log p(w|Context(w))$

其中，$w$是上下文集合$\mathcal{C}$里的词，$Context(w)$是$w$的上下文。

输入层是上下文词语的词向量。（我们的参数并非是词向量，我们的主要目标是训练这个模型的参数，词向量是这个模型的副产物，最初任何词向量都是随机值）

紧接着来到投影层，我们在投影层将上下文的向量加和。

最后我们得到输出向量，并给出概率最大的候选词。

特别地，对于输出的向量$y$，我们对该向量softmax化来得到最终的概率。

$softmax(\begin{bmatrix}
p(w_i = 1|x_i,\theta)
\\
p(w_i = 2|x_i,\theta)
\\
...
\\
p(w_i = k|x_i, \theta)
\end{bmatrix}) = \frac{1}{\sum_{i=1}^ke^{p(w_i = k|x_i,\theta)}}\begin{bmatrix}
e^{p(w_i = 1|x_i,\theta)}
\\
e^{p(w_i = 2|x_i,\theta)}
\\
...
\\
e^{p(w_i = k|x_i, \theta)}
\end{bmatrix}$

但是我们的候选词库是整个语料库，所以我们直接在$|\mathcal{C}|$大小的范围内选，无论是矩阵乘法还是最终选词时间复杂度都是非常难以接受的。

因此我们有了别的思路，因为二类选择问题可以推广至n类问题，所以我们干脆建一棵哈夫曼树，让每一个叶子节点代表一个候选词，然后按照从树根到叶子节点遍历的方式得到每一个叶子的概率。

这也就是

### Hierarchical Softmax

我们引入一些叙述，$\boldsymbol{x_w}$表示$w$的上下文向量,

$p^w$表示从根节点到词语$w$的路径，

$p^w_1,p^w_2,...,p^w_{|p^w|}$表示这个路径上的各个点，编号按照根节点到该节点进行编号。

$d^w_2,d^w_2,...,d^w_{|p^w|} \in \{0,1\}$表示这些点的编号，特别地，根节点没有编号，并且，向左为$0$，向右为$1$

$\theta^w_{2},\theta^w_{3},...,\theta^w_{|p^w|}$表示这些点的参数向量。

于是根据条件概率，$p(w|Context(w)) = \prod_{i=2}^{|p^w|} p(p^w_i|\boldsymbol{x}_w ,\theta^w_i)$

特别地，$p(p^w_i) = \left\{\begin{aligned} \sigma(\boldsymbol{x_w}\cdot \theta^w_i), & d^w_i = 1 \\ 1 - \sigma(\boldsymbol{x_w}\cdot \theta^w_i), & d^w_i = 0
\end{aligned}\right.$，其中$\sigma(x)$是sigmoid函数，是softmax的二元形式。

那么就有$p(p^w_i) = \sigma(\boldsymbol{x_w}\cdot \theta^w_i)^{d^w_i}( 1 - \sigma(\boldsymbol{x_w}\cdot \theta^w_i))^{1 - d^w_i}$

将他们都乘起来$p(w|Context(w)) = \prod_{i=2}^{|p_w|}p(p^w_i) = \prod_{i=2}^{|p^w|}\sigma(\boldsymbol{x_w}\cdot \theta^w_i)^{d^w_i}( 1 - \sigma(\boldsymbol{x_w}\cdot \theta^w_i))^{1 - d^w_i}$

取对数，$\log p(w|Context(w)) = \sum_{i = 2}^{|p^w|}(d^w_i \log(\sigma(\boldsymbol{x_w}\cdot \theta^w_i)) + (1 - d^w_i) \log(1 - \sigma(\boldsymbol{x_w}\cdot \theta^w_i)))$

$\sigma(x) = \frac{1}{1 + e^{-x}}$

求导，$\sigma'(x) = \frac{e^{-x}}{1 + 2e^{-x} + e^{-2x}} = \frac{1}{e^x + 2 + e^{-x}}$

$e^{-x} = \frac{1 - \sigma(x)}{\sigma(x)}$，带入上式，$LHS = \frac{1}{\frac{\sigma(x)}{1 - \sigma(x)} + 2 + \frac{1 - \sigma(x)}{\sigma(x)}} = \frac{\sigma(x) - \sigma(x)^2}{\sigma(x)^2 + 2\sigma(x) - 2\sigma(x)^2 + 1 - 2\sigma(x) + \sigma(x)^2} = \sigma(x)(1 - \sigma(x))$

所以回到取对数的环节，原式求导$\frac{\partial \log p}{\partial \theta^w_i} = d^w_i \boldsymbol{x_w} \sigma(\boldsymbol{x_w}\cdot \theta^w_i)(1 - \sigma(\boldsymbol{x_w}\cdot \theta^w_i)) \frac{1}{\sigma(\boldsymbol{x_w}\cdot \theta^w_i)} - (1 - d^w_i) \boldsymbol{x_w}\sigma(1 - \sigma(\boldsymbol{x_w}\cdot \theta^w_i))\sigma(\boldsymbol{x_w}\cdot \theta^w_i) \frac{1}{1 - \sigma(\boldsymbol{x_w}\cdot \theta^w_i)} = d^w_i\boldsymbol{x_w}(1 - \sigma(\boldsymbol{x_w}\cdot \theta^w_i)) - (1 - d^w_i)\boldsymbol{x_w}\sigma(\boldsymbol{x_w}\cdot \theta^w_i) = (d^w_i - \sigma(\boldsymbol{x_w}\cdot \theta^w_i) - 1)\boldsymbol{x_w}$

我们观察到$\theta^w_i$和$\boldsymbol{x_w}$是轮换对称的，所以$\frac{\partial \log p}{\partial \boldsymbol{x_w}} = (d^w_i - \sigma(\boldsymbol{x_w}\cdot \theta^w_i) - 1)\theta^w_i$

每次学习后，我们只需要$\theta^w_i := \theta^w_i + \eta (d^w_i - \sigma(\boldsymbol{x_w}\cdot \theta^w_i) - 1)\boldsymbol{x_w}$，$\boldsymbol{x} := \boldsymbol{x} + (d^w_i - \sigma(\boldsymbol{x_w}\cdot \theta^w_i) - 1)\theta^w_i$即可。

问题在于，$\boldsymbol{x_w}$是一个整体，是若干个词向量的向量之和，所以我们均摊一下对于每一个词的影响即可。
