# Get a root privilege by modifying a current program's cred structure.
# Vuln1. strcpy_from_user do not copy a null (differ from strcpy)
# Vuln2. Buffer overflow in note
# Exploit
## 1. Overwrite a current->cred pointer to node pointer.
## 2. Create a fake cred structure
### When create a fake cred structure, you should take care about cred->security, cred->user's usage field (ref count).
## 3. Read a flag
