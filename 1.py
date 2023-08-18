from matplotlib import pyplot as pt
import pandas
df = pandas.read_csv("1.csv")
l1 = df.iloc[:,0:1].values.tolist()
l2 = df.iloc[:,1:2].values.tolist()
l2 = df.iloc[:,2:3].values.tolist()
print(l1)
print(l2)
pt.plot(l1,l2)
pt.xlabel("User Count")
pt.ylabel("Response Time")
pt.title("Think time = 0.5 sec, Test Durationn = 60 sec")
pt.show()