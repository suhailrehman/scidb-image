#Assigned Tasks

Last week, element-wise multiplication in SciDB was really poor in performance because of the use of join operation over multiple arrays. Explore avenues to optimize.

Goals: Further explore apporaches to element-wise image array operations to improve speed and compare against hand-coded C baseline

1.  Handcoded C baseline
2.  Perform image averaging by extending image array to extra dimension for n images, flatten the image into average image in DB
3.  Timing and Comparison

#Required Setup
1.	Working SciDB 15.7 installation (compiled from source and installed) [Download Page - Requires Registration](http://paradigm4.com/forum/viewtopic.php?f=14&t=1672&sid=6e52162f4ef747ad2f304764081fe8fd)
2.	Working SciDB.py setup (latest snapshot from github commit `c89902fe5e` - [Github Repo](https://github.com/Paradigm4/SciDB-Py) )
3.	scikit-image processing library in python [scikit-image homepage](http://scikit-image.org/)

#References

[CImg Library](http://cimg.eu/)

[Element-Wise Multiplicaition in SciDB](http://paradigm4.com/forum/viewtopic.php?p=1838)
