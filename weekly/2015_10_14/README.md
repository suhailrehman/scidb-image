#Assigned Tasks

Last week, element-wise multiplication in SciDB was really poor in performance because of the use of join operation over multiple arrays. Explore avenues to optimize.

Goals: Further explore apporaches to element-wise image array operations to improve speed and compare against hand-coded C baseline

1.  Handcoded C baseline
2.  Perform image averaging by extending image array to extra dimension for n images, flatten the image into average image in DB
3.  Timing and Comparison

#Findings
1.	Average Query for a 4-D array of images:
`aggregate(array_name,avg(f0),i1,i2);`
2.	

#References

[CImg Library](http://cimg.eu/)

[Element-Wise Multiplicaition in SciDB](http://paradigm4.com/forum/viewtopic.php?p=1838)
