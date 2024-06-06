import { ref } from "vue"

export const state = {
  currentUser: ref(null),
  loginState: ref(0)
}

export const actions = {

  setCurrentUser: (data) => {
    state.currentUser.value = data
  },
  setLoginState : (data) => {
    state.loginState.value = data
  }
}

export default {
  state,
  actions
}