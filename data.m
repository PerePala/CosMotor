clear all

T=1;    % 1 seconds
A=800;  % 800 steps (1 revolutions)

nTargets=11;   % Points in half period

D=T/nTargets;

us=1e-6;
ms=1e-3;
t=[0:D:T];
n=A/2*(1-cos(pi*t/T));

dn=diff(n);
out_dn=round(dn);

s=D./out_dn/us;

out=round([t(2:end)'/ms , (out_dn)',s' ])

% format as nice string
s='';
for ix=[1:(length(out)+1)/2]
  s=[s, '{ ',sprintf('%4d',out(ix,2)), ' , ', sprintf('%4d',out(ix,3)) " }, \n"];
endfor

% save to text file
fid=fopen ("s.txt", "w");
fputs(fid,s);
fclose (fid);

