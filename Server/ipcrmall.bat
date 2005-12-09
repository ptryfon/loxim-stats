for i in `ipcs | cut -f2 -d" "`i; do
  ipcrm shm $i
  ipcrm sem $i
  ipcrm msg $i
done