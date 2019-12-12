% This function estimates the RTF using Covariance Substraction
function rtf_estimate = rtf_cov_sub(Ryy, Rnn, e)

Rxx = Ryy - Rnn; % Speech only correlation matrix estimate
[V D]= eig(Rxx,'vector');  % AV = VD with D the diagonal eigenvalue matrix and V the set of right eigenvectors
[I,J]=find(D==max(D));
v_prin=V(:,I);
v_prin_norm = v_prin/sqrt(sum(abs(v_prin).^2));
rtf_estimate = v_prin_norm/(e*v_prin_norm);

end
