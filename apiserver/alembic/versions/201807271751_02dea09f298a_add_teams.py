"""Add teams

Revision ID: 02dea09f298a
Revises: b298f5f566fa
Create Date: 2018-07-27 17:51:21.259443+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import mysql


# revision identifiers, used by Alembic.
revision = '02dea09f298a'
down_revision = 'b298f5f566fa'
branch_labels = None
depends_on = None


def upgrade():
    op.create_table('team',
                    sa.Column('id', sa.Integer(), nullable=False),
                    sa.Column('created', sa.TIMESTAMP(timezone=True), server_default=sa.text('CURRENT_TIMESTAMP'), nullable=False),
                    sa.Column('name', sa.String(length=32), server_default=sa.text('CURRENT_TIMESTAMP'), nullable=False),
                    sa.Column('verification_code', sa.String(length=32), nullable=False),
                    sa.Column('leader_id', sa.Integer(), autoincrement=False, nullable=False),
                    sa.ForeignKeyConstraint(['leader_id'], ['user.id'], name='team_leader_id_user_ibfk'),
                    sa.PrimaryKeyConstraint('id'),
    )
    op.add_column('user',
                  sa.Column('team_id', sa.Integer(), autoincrement=False, nullable=True))
    op.create_foreign_key('user_team_id_team_fk', 'user', 'team', ['team_id'], ['id'])


def downgrade():
    op.drop_constraint('user_team_id_team_fk', 'user')
    op.drop_column('user', 'team_id')
    op.drop_table('team')
