@echo off

set new_key=f
if %BOKEH_SECRET_KEY%=="" set new_key=y
if "%1"=="create_key" set new_key=y

if "%new_key%"=="y" (
    for /f %%f in ('call bokeh secret') do (
        set BOKEH_SECRET_KEY=%%f
        echo "New Bokeh secret key created."
    )
) else  (
    echo "Bokeh secret key exists."
)


setlocal
set address=10.10.10.10
set port=80

python manage.py runserver %address%:%port%