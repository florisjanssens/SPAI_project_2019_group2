
% 30/09/2016
% Plotting the sound scenario with external mic
% generated from contNR_externalmic

vpostr = v_pos;
xnoise = vpostr(1,:);
ynoise = vpostr(2,:);

xsource = s_pos(1,:);
ysource = s_pos(2,:);

xmic_array = pm(1,1:M);
ymic_array = pm(2,1:M);


figure;
plot(xmic_array,ymic_array,'ro', xsource, ysource, 'ms', xnoise, ynoise, 'kx', 'MarkerSize',14)
xlim([0 room_dim(1)])
ylim([0 room_dim(2)])

position = [100, 100, 700, 300];
set(gcf, 'position', position)
set(gca, 'fontsize', 14)
set(gcf,'color','w');
grid on
xlabel('x-coord (m)', 'fontsize', 14)
ylabel('y-coord (m)', 'fontsize', 14)
title({'Room Acs. Scene'})
legend('Mic. Array','Speech Source','Local Noise')
view([270 90])

