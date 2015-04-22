mkdir vecs
call :create 0
call :create 1
call :create 2
call :create 3
call :create 4
call :create 5
call :create 6
call :create 7
call :create 8
call :create 9
call :create 10
call :create 11
call :create 12
call :create 13
call :create 14
call :create 15
call :create 16
call :create 17
call :create 18
call :create 19
call :create 20
exit /b

:create
opencv_createsamples -info %1.dat -vec vecs/%1.vec -w 8 -h 16
exit /b

