import { createApp } from 'vue';
import App from './App.vue';
import router from './router';
import vuetify  from './plugins/vuetify'; 
import { auth } from './firebaseConfig';

// Crea una instancia de la aplicación
const app = createApp(App);

// Usa Vuetify en la aplicación
app.use(vuetify);

// Usa el enrutador en la aplicación
app.use(router);

// Monta la aplicación en el elemento del DOM con el ID 'app'
app.mount('#app');

// Configura la protección de rutas
router.beforeEach((to, from, next) => {
  const requiresAuth = to.matched.some(record => record.meta.requiresAuth);
  const isAuthenticated = auth.currentUser;

  if (requiresAuth && !isAuthenticated) {
    next('/');
  } else {
    next();
  }
});

