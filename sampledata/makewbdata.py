import numpy as np
import matplotlib.pyplot as plt 

dur = 10
wbfs = 10**5

wbdata_fft = np.random.randn(2*dur*wbfs).view(np.complex128)

# signal 1:variable length bursts
alldata = {}
F = 0
while True:
    guard1fs = np.random.randint(1000,2000)
    guard2fs = np.random.randint(1000,2000)
    nbfs = np.random.randint(1000, 3000)
    f1 = F+guard1fs
    f2 = f1 + nbfs 
    F = f2 + guard2fs

    if F > wbfs:
        break

    nbdata_all = []

    burstminlength = 0.1
    burstmaxlength = 0.5
    silenceminlength = 0.1
    silencemaxlength = 0.5
    T = 0
    targetlength = nbfs * dur
    while True:
        sil1 = np.zeros(np.random.randint(int(silenceminlength*nbfs), int(silencemaxlength*nbfs)), dtype=np.complex128)
        nbdata = 5*np.random.randn(np.random.randint(int(burstminlength*nbfs), int(burstmaxlength*nbfs))*2).view(np.complex128)
        sil2 = np.zeros(np.random.randint(int(silenceminlength*nbfs), int(silencemaxlength*nbfs)), dtype=np.complex128)

        additional_length = len(sil1) + len(nbdata) + len(sil2)

        if T + additional_length < targetlength:
            nbdata_all += [sil1, nbdata, sil2]
            T += additional_length
        else:
            sil = np.zeros(targetlength - T , dtype=np.complex128)
            nbdata_all.append(sil)
            T += len(sil)
            break

    
    f_idx1 = f1*dur
    f_idx2 = f2*dur


    nbdata_all = np.concatenate(nbdata_all)
    wbdata_fft[f_idx1:f_idx2] += np.fft.fftshift(np.fft.fft(nbdata_all, norm='ortho'))

wbdata = np.fft.ifft(np.fft.ifftshift(wbdata_fft), norm='ortho')
wbdata = wbdata/ np.max(np.abs(wbdata)) * 2**15
wbdata = wbdata.view(np.float64).astype(np.int16)
for t in range(dur):
    wbdata[2*wbfs*t:(t+1)*2*wbfs].tofile(f"randombursts{t:02d}.bin")
