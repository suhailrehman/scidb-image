from scidbpy import connect

sdb = connect('http://localhost:8080/')

#create random weights
for array in sdb.list_arrays():
     sdb.query("remove(%s)"%array)

