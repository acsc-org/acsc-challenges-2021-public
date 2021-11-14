import Koa from 'koa';
import Router from '@koa/router';
import auth from 'koa-basic-auth';
import bodyParser from 'koa-bodyparser';
import child_process from 'child_process';

const settings = {};

const style = `<style>
body { padding: 2rem; }
.form input[type=text] { padding: .5rem 1rem; font-size: 1rem; display: block; margin-bottom: 1rem; }
.form input[type=submit] { display: block; margin-bottom: 1rem; color: #fff; background-color: #000; padding: .5rem 1rem; font-size: 1rem; border: none; }
.color-setting { margin-bottom: 1rem; }
.cowsay { font-size: 2rem; background: #beead6; padding: 0.5rem 1rem; }
</style>`;

const app = new Koa();
const router = new Router();

// basic auth
if (process.env.CS_USERNAME && process.env.CS_PASSWORD) {
  app.use(auth({
    name: process.env.CS_USERNAME,
    pass: process.env.CS_PASSWORD
  }))
}

app.use(async (ctx, next) => {
  ctx.state.user = ctx.cookies.get('username');
  await next();
});

router.get('/', (ctx, next) => {
  ctx.body = `
${style}
<h1>Welcome to Cowsay as a Service</h1>
<p>Before start the service, please enter your name.</p>
<form action="/cowsay" method="GET" class="form">
  <input type="text" name="user" placeholder="Username">
  <input type="submit" value="Login">
</form>
<script>
document.querySelector('form').addEventListener('submit', () => {
  const username = document.querySelector('input[name="user"]').value;
  document.cookie = 'username=' + username;
});
</script>
`;
  next();
});

router.get('/cowsay', (ctx, next) => {
  const setting = settings[ctx.state.user];
  const color = setting?.color || '#000000';

  let cowsay = '';
  if (ctx.request.query.say) {
    const result = child_process.spawnSync('/usr/games/cowsay', [ctx.request.query.say], { timeout: 500 });
    cowsay = result.stdout.toString();
  }

  ctx.body = `
${style}
<h1>Cowsay as a Service</h1>

<details class="color-setting">
  <summary>Color Preferences</summary>
  <form action="/setting/color" method="POST">
    <input type="color" name="value" value="${color}">
    <input type="submit" value="Change Color">
  </form>
</details>

<form action="/cowsay" method="GET" class="form">
  <input type="text" name="say" placeholder="hello">
  <input type="submit" value="Say">
</form>

<pre style="color: ${color}" class="cowsay">
${cowsay}
</pre>
`;
});

router.post('/setting/:name', (ctx, next) => {
  if (!settings[ctx.state.user]) {
    settings[ctx.state.user] = {};
  }
  const setting = settings[ctx.state.user];
  setting[ctx.params.name] = ctx.request.body.value;
  ctx.redirect('/cowsay');
});

app.use(bodyParser());
app.use(router.routes());
app.use(router.allowedMethods());
app.listen(3000);
