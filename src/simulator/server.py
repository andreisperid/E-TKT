from aiohttp import web
import asyncio
import os

# The port the webserver will start on
PORT = 80

#  How long it shoudl take to simulate printing a character
PRINT_CHARACTER_SECONDS = 2

# How long it should take to simulate any other action (eg cut, feed, etc)
OTHER_COMMAND_SECONDS = 5

class Server:
    """
    Simulates a physical E-TKT device by serving web interface, locking during
    printing, and reporting print progress.
    """
    def __init__(self):
        self.busy = False
        self.align = 5
        self.force = 5
        self.command = ""
        self.value = ""
        self.progress = 100

    async def start(self):
        app = web.Application()
        app.add_routes([
            web.get('/api/status', self.status),
            web.post('/api/task', self.task),
            web.get('/', self.index),
            web.static("/", self.relativePath('../../data/')),
        ])
        runner = web.AppRunner(app)
        await runner.setup()
        site = web.TCPSite(runner, "0.0.0.0", PORT)
        print(f"Starting webserver, http://localhost:{PORT}")
        await site.start()

    async def index(self, request):
        return web.FileResponse(self.relativePath('../../data/index.html'))

    async def status(self, request):
        return web.json_response({
            'command': self.command,
            'progress': self.progress,
            'busy': self.busy,
            'force': self.force,
            'align': self.align
        })

    async def task(self, request: web.Request):
        data = await request.json()
        if self.busy:
            return web.json_response(status=400)
        self.busy = True
        self.command = data['parameter']
        self.value = data['value']
        asyncio.create_task(self.process_command())
        return web.json_response({})


    async def process_command(self):
        self.progress = 0
        if self.command == "tag":
            # Simulate printing, update the progress for each character
            for x in range(len(self.value) + 1):
                await asyncio.sleep(1)
                self.progress = int(100 * x / len(self.value))
            await asyncio.sleep(PRINT_CHARACTER_SECONDS)
        else:
            # Any other command takes the same amount of time
            await asyncio.sleep(OTHER_COMMAND_SECONDS)
        self.busy = False

    def relativePath(self, path):
        return os.path.join(os.path.dirname(__file__), path)
