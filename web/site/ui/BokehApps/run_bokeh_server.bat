@echo off

setlocal enabledelayedexpansion

set websocket=10.10.10.10
set address=127.0.0.1
set bokeh_files=

for %%f in (*.py) do (
    if .!bokeh_files!==. (
        set bokeh_files=%%f 
    ) else (
        set bokeh_files=!bokeh_files! %%f
    )
)

bokeh serve !bokeh_files! --allow-websocket-origin %websocket% --address %address%

EXIT /B 0