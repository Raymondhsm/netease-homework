using System;
using UnityEngine;

[Serializable]
public class WeaponController
{
	[SerializeField]
	private int _weaponNum;
	[SerializeField]
	private int[] _weaponMaxBullet;
	[SerializeField]
	private int[] _weaponTotalBullet;
	[SerializeField]
	private int[] _weaponCurrBullet;
	[SerializeField]
	private int[] _weaponMode;

	private int _weaponIndex;

	public WeaponController()
	{
		_weaponIndex = 0;
	}

	public int Reload()
	{
		int usedBullet = _weaponMaxBullet[_weaponIndex] - _weaponCurrBullet[_weaponIndex];
		if(usedBullet > _weaponTotalBullet[_weaponIndex])
		{
			_weaponCurrBullet[_weaponIndex] += _weaponTotalBullet[_weaponIndex];
			_weaponTotalBullet[_weaponIndex] = 0;
		}
		else
		{
			_weaponTotalBullet[_weaponIndex] -= usedBullet;
			_weaponCurrBullet[_weaponIndex] = _weaponMaxBullet[_weaponIndex];
		}
		return _weaponCurrBullet[_weaponIndex];
	}

	public bool ChangeWeapon(int index)
	{
		if (index < 0 || index >= _weaponMaxBullet.Length)
			return false;

		_weaponIndex = index;
		return true;
	}

	public void NextWeapon()
	{
		_weaponIndex = (_weaponIndex + 1) % _weaponNum;
	}

	public bool Shoot()
	{
		if (_weaponCurrBullet[_weaponIndex] - 1 < 0)
			return false;

		_weaponCurrBullet[_weaponIndex]--;
		return true;
	}

	public bool canShoot()
	{
		if (_weaponCurrBullet[_weaponIndex] - 1 < 0)
			return false;
		return true;
	}

	public void ChangeMode()
	{
		_weaponMode[_weaponIndex] = (_weaponMode[_weaponIndex] + 1) % 2;
	}

	public int CurrBullet
	{
		get{ return _weaponCurrBullet[_weaponIndex]; }
	}

	public int TotalBullet
	{
		set { _weaponTotalBullet[_weaponIndex] = value; }
		get { return _weaponTotalBullet[_weaponIndex]; }
	}

	public int WeaponMode
	{
		get { return _weaponMode[_weaponIndex]; }
	}
}
